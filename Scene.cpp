#include "Scene.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


/**
 * Jeden vrchol ve VBO slozeny ze souradnic, normaly a souradnic textury
 */
typedef struct VBOENTRY {
	float x, y, z;		// souradnice
	float nx, ny, nz;	// normala
	float tx, ty, tz;	// tangenta
	float u, v;			// textura
} VBOENTRY;




/////////////////////////////////////////////////////////////////////////


Scene::Scene(BaseApp& parentApp) : application(parentApp)
{	
}



Scene::~Scene() 
{
	// uvolnit obsah kontejneru
	for (vector<ModelContainer*>::iterator it = containers.begin(); it != containers.end(); it++)
	{
		delete (*it);
	}

	// uvolnit buffery
	if (VBOs.size() > 0)
		glDeleteBuffers(VBOs.size(), &VBOs[0]);
	if (EBOs.size() > 0)
		glDeleteBuffers(EBOs.size(), &EBOs[0]);
}



void Scene::addModelContainer(ModelContainer* container)
{
	containers.push_back(container);
}


std::vector<ModelContainer*> &Scene::getModelContainers()
{
	return containers;
}


void Scene::init()
{
	buildBufferObjects();


	// najit vsechna svetla a aktivovat je
	for (vector<ModelContainer*>::iterator it = containers.begin(); it != containers.end(); it++)
	{
		GLenum lightEnums[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
		unsigned int i = 0;
		if ((*it)->getLights().size() > 0 && i < 8)
		{
			glEnable(lightEnums[i]);
			Light l = (*it)->getLights().at(i);
			glLightfv(lightEnums[i], GL_POSITION, &l.Position().x);
		}
	}


	// zoptimalizovat vsechny konterjnery
	for (vector<ModelContainer*>::iterator it = containers.begin(); it != containers.end(); it++)
	{
		(*it)->optimizeDrawingQueue();
	}


#if 0
	cout << "Scene: " << containers.size() << " containers" << endl;
	for (unsigned int i = 0; i < containers.size(); i++)
	{
		cout << i << endl;
		cout << "\tmodels: " << containers[i]->modelsCount() << "\tverts: " << containers[i]->verticesCount() << "\tfaces: " << containers[i]->facesCount() << endl;
	}
#endif

}


void Scene::buildBufferObjects()
{
	for (vector<ModelContainer*>::iterator it = containers.begin(); it != containers.end(); it++)
	{
		ModelContainer* container = (*it);
		vector<BaseModel*> models = container->getModels();
		
		// VBO /////////////////////////////////////////////////////////
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			VBOs.push_back(vbo);

			// alokovat misto v bufferu
			glBufferData(GL_ARRAY_BUFFER, container->verticesCount() * sizeof(VBOENTRY), NULL, GL_STATIC_DRAW);
			
			// mapovani pameti
			VBOENTRY* mapping = (VBOENTRY*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// naplneni daty
			for (vector<BaseModel*>::iterator modelit = models.begin(); modelit != models.end(); modelit++)
			{
				BaseModel* model = (*modelit);

				for (vector<Mesh*>::iterator meshit = model->getMeshes().begin(); meshit != model->getMeshes().end(); meshit++)
				{
					Mesh* mesh = (*meshit);

					// spocitat tangenty a normaly
					mesh->computeTangentsAndNormals();

					for (unsigned int i = 0; i < mesh->getVertices().size(); i++)
					{
						glm::vec3 vert = mesh->getVertices()[i];
						glm::vec3 norm = mesh->getNormals()[i];
						glm::vec3 tan = mesh->getTangents()[i];

						glm::vec2 tex(0, 0);
						if (mesh->getTexCoords().size() > i)
							tex = mesh->getTexCoords()[i];

						// zapsat data a posunout ukazatel na nasledujici volne misto
						VBOENTRY e = {
							vert.x, vert.y, vert.z, 
							norm.x, norm.y, norm.z,
							tan.x, tan.y, tan.z,
							tex.x, tex.y
						};
						
						*mapping = e;
						mapping++;
					}
				}
			}

			// presun dat do graficke pameti
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}

		// EBO ////////////////////////////////////////////////////////////
		{
			GLuint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			EBOs.push_back(ebo);

			// alokovat misto v bufferu
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, container->facesCount() * 3 * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

			// mapovani pameti
			unsigned int* mapping = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
			unsigned int writingIndex = 0; // index do mapovane pameti, kam se bude zapisovat
			unsigned int indexOffset = 0; // kolik indexu uz bylo zapsano; nutno pricitat pocty vrcholu (!) predchozich meshi

			// naplneni daty
			for (vector<BaseModel*>::iterator modelit = models.begin(); modelit != models.end(); modelit++)
			{
				BaseModel* model = (*modelit);
				
				for (unsigned int mi = 0; mi < model->getMeshes().size(); mi++)
				{
					// kazda mesh ma indexy cislovane od 0
					Mesh* mesh = model->getMeshes()[mi];

					for (unsigned int i = 0; i < mesh->getFaces().size(); i++)
					{
						glm::ivec3 face = mesh->getFaces().at(i);
						
						// zapsat data a posunout ukazatel na nasledujici volne misto
						mapping[writingIndex + 0] = (unsigned int)face.x + indexOffset;
						mapping[writingIndex + 1] = (unsigned int)face.y + indexOffset;
						mapping[writingIndex + 2] = (unsigned int)face.z + indexOffset;

						writingIndex += 3;					
					}	
					
					// pricist pocet vrcholu (!) teto meshe; pristi mesh musi nalezite precislovat indexy
					indexOffset += mesh->getVertices().size();
				}				
			}

			// presun dat do graficke pameti
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}
		
	}	
}




void Scene::draw() 
{	
	string activeMaterial = "?_dummy_?";
	ShaderManager::PROGRAMBINDING activeBinding;
	
	// pripravit si pole svetel ze vsech kontejneru
	vector<glm::vec4> lights;

	for (vector<ModelContainer*>::iterator it = containers.begin(); it != containers.end(); it++)
	{
		GLenum lightEnums[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
		unsigned int i = 0;
		if ((*it)->getLights().size() > 0 && i < 8)
		{
			glEnable(lightEnums[i]);
			Light l = (*it)->getLights().at(i);
			glLightfv(lightEnums[i], GL_POSITION, &l.Position().x);
		}
	}
		
	for (unsigned int i = 0; i < containers.size(); i++)
	{
		vector<Light> containerLights = containers.at(i)->getLights();
		for (vector<Light>::iterator it = containerLights.begin(); it != containerLights.end(); it++)
		{
			lights.push_back((*it).Position());
			lights.push_back((*it).Diffuse());
			lights.push_back((*it).Ambient());
		}
	}



	// samotne kresleni
	for (unsigned int i = 0; i < containers.size(); i++)
	{						
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);

		// postupne provadet kreslici frontu meshi kontejneru, slozenou z kreslene meshe a jeji matice;
		// pokud probehla optimalizace, budou meshe navic serazene podle pouzivaneho materialu
		vector<ModelContainer::MESHDRAWINGQUEUEITEM> drawingQueue = containers[i]->getMeshDrawingQueue();
		
		unsigned int counter = 0;

		for (vector<ModelContainer::MESHDRAWINGQUEUEITEM>::iterator it = drawingQueue.begin(); it != drawingQueue.end(); it++)
		{
			Mesh* mesh = (*it).mesh;

			// prepinat shadery jen pokud je treba
			if (activeMaterial != mesh->getMaterialName())
			{				
				activeMaterial = mesh->getMaterialName();
				activeBinding = ShaderManager::useProgram(activeMaterial);					

				// nastavit buffery
				glEnableVertexAttribArray(activeBinding.positionAttrib);
				glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, x));

				glEnableVertexAttribArray(activeBinding.normalAttrib);
				glVertexAttribPointer(activeBinding.normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, nx));

				glEnableVertexAttribArray(activeBinding.tangentAttrib);
				glVertexAttribPointer(activeBinding.tangentAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, tx));

				glEnableVertexAttribArray(activeBinding.texposAttrib);
				glVertexAttribPointer(activeBinding.texposAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, u));

				// nastavit svetla
				glUniform1i(activeBinding.iEnabledLightsUniform, (unsigned int)(lights.size() / 3));
				glUniform4fv(activeBinding.vLightsUniform, lights.size(), &(lights[0].x));

				// ? glUniform4fv(activeBinding.vLightsUniform, lights.size() * 4, &(lights[0].x));

				counter++; // pomocne pocitadlo zmen shaderu v jednom snimku
			}				

			// pohledova matice
			glm::mat4 mView = application.getCamera()->GetMatrix();
			glUniformMatrix4fv(activeBinding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));

			// projekcni matice
			glm::mat4 mProjection = glm::perspective(45.0f, (float)application.getWindowAspectRatio(), 0.1f, 1000.0f);
			glUniformMatrix4fv(activeBinding.mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjection));
	
			// nastaveni kamery
			glm::vec3 eye = application.getCamera()->getEye();
			glm::vec3 sight = application.getCamera()->getTarget();
			GLuint eyeUniform = glGetUniformLocation(activeBinding.program, "eye");
			GLuint sightUniform = glGetUniformLocation(activeBinding.program, "sight");
			glUniform3f(eyeUniform, eye.x, eye.y, eye.z);
			glUniform3f(sightUniform, sight.x, sight.y, sight.z);

			// modelova matice
			glUniformMatrix4fv(activeBinding.mModelUniform, 1, GL_FALSE, glm::value_ptr((*it).matrix));

			// pomocna matice pro vypocty osvetleni - znacne snizeni fps!
			glm::mat3 mSubModelView = glm::mat3(mView) * glm::mat3((*it).matrix);
			glm::mat3 mMVInverseTranspose = glm::transpose(glm::inverse(mSubModelView)); // transpose(inverse(modelview))
			glUniformMatrix3fv(activeBinding.mMVInverseTranspose, 1, GL_FALSE, glm::value_ptr(mMVInverseTranspose));

			// samotne vykresleni
			unsigned int count = mesh->getFaces().size() * 3;
			unsigned int offset = containers[i]->getMeshIndexOffset(mesh);
			
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)) );			
		}		
		
		// vynuti opetovne nastaveni shaderu pro kazdy kontejner,
		// muze byt totiz nutne prenastavit ukazatele do bufferu atp.
		activeMaterial = "?_dummy_?";

		//cout << "prepnuti krat " << counter << endl;
	}	

	// obnovit vychozi binding bufferu
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
