#include "Scene.h"

using namespace std;


/**
 * Jeden vrchol ve VBO slozeny ze souradnic, normaly a souradnic textury
 */
typedef struct VBOENTRY {
	float x, y, z;		// souradnice
	float nx, ny, nz;	// normala
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

					// spocitat normaly
					mesh->computeNormals();

					for (unsigned int i = 0; i < mesh->getVertices().size(); i++)
					{
						glm::vec3 vert = mesh->getVertices()[i];
						glm::vec3 norm = mesh->getNormals()[i];

						glm::vec2 tex(0, 0);
						if (mesh->getTexCoords().size() > i)
							tex = mesh->getTexCoords()[i];

						// zapsat data a posunout ukazatel na nasledujici volne misto
						VBOENTRY e = {
							vert.x, vert.y, vert.z, 
							norm.x, norm.y, norm.z, 
							tex.x, tex.y
						};
						
						*mapping = e;
						mapping++;

						//cout << "v[" << i << "]\t" << vert.x << "\t" << vert.y << "\t" << vert.z << endl;
						//cout << "n[" << i << "]\t" << norm.x << "\t" << norm.y << "\t" << norm.z << endl;
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
						glm::vec3 face = mesh->getFaces().at(i);
						
						// zapsat data a posunout ukazatel na nasledujici volne misto
						mapping[writingIndex + 0] = (unsigned int)face.x + indexOffset;
						mapping[writingIndex + 1] = (unsigned int)face.y + indexOffset;
						mapping[writingIndex + 2] = (unsigned int)face.z + indexOffset;

						if (0) {
							cout << "writingIndex " << writingIndex << "\t" << mapping[writingIndex + 0] << "\t" << mapping[writingIndex + 1] << "\t" << mapping[writingIndex + 2] << endl;
							cout << "original     " << writingIndex << "\t" << (unsigned int)face.x << "\t" << (unsigned int)face.y << "\t" << (unsigned int)face.z << endl;
						}
						
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
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);

		// postupne provadet kreslici frontu kontejneru, slozenou z kresleneho modelu a jeho matice
		// kazdy model se pak sklada z meshi, kde kazda ma nejaky material (shader)
		vector<ModelContainer::DRAWINGQUEUEITEM> drawingQueue = containers[i]->getDrawingQueue();

		for (vector<ModelContainer::DRAWINGQUEUEITEM>::iterator it = drawingQueue.begin(); it != drawingQueue.end(); it++)
		{
			
			unsigned int meshOffset = 0;

			for (vector<Mesh*>::iterator meshIt = (*it).model->getMeshes().begin(); meshIt != (*it).model->getMeshes().end(); meshIt++)
			{
				// prepinat shadery jen pokud je treba
				if (activeMaterial != (*meshIt)->getMaterialName())
				{				
					activeMaterial = (*meshIt)->getMaterialName();
					activeBinding = ShaderManager::useProgram(activeMaterial);
				}

				// nastavi pohledove matice, kameru, atd.
				setProgramUniforms(activeBinding);

				glEnableVertexAttribArray(activeBinding.positionAttrib);
				glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, x));

				glEnableVertexAttribArray(activeBinding.normalAttrib);
				glVertexAttribPointer(activeBinding.normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, nx));

				glEnableVertexAttribArray(activeBinding.texposAttrib);
				glVertexAttribPointer(activeBinding.texposAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, u));

				glUniformMatrix4fv(activeBinding.mModelUniform, 1, GL_FALSE, glm::value_ptr((*it).matrix) );
						
				unsigned int count = (*meshIt)->getFaces().size() * 3;
				unsigned int offset = containers[i]->getModelIndexOffset((*it).model) + meshOffset;
			
				glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)) );

				meshOffset += count;
			}
			
		}		

	}

}



void Scene::setProgramUniforms(ShaderManager::PROGRAMBINDING binding)
{
	GLuint program = binding.program;

    // pohledova matice
	glm::mat4 mView = application.getCamera()->GetMatrix();
	glUniformMatrix4fv(binding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));

	// projekcni matice
	glm::mat4 mProjection = glm::perspective(45.0f, (float)application.getWindowAspectRatio(), 1.0f, 1000.0f);
	glUniformMatrix4fv(binding.mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjection));

	// nastaveni svetel - !!! tmp reseni !!!
	GLint lights[] = {
		1, 0, 0, 0, 0, 0, 0, 0
	};
	GLuint enabledLightsUniform = glGetUniformLocation(program, "enabledLights");
	glUniform1iv(enabledLightsUniform, 8, lights);

	// nastaveni kamery
	glm::vec3 eye = application.getCamera()->getEye();
	glm::vec3 sight = application.getCamera()->getTarget();
	GLuint eyeUniform = glGetUniformLocation(program, "eye");
	GLuint sightUniform = glGetUniformLocation(program, "sight");
	glUniform3f(eyeUniform, eye.x, eye.y, eye.z);
	glUniform3f(sightUniform, sight.x, sight.y, sight.z);
}
