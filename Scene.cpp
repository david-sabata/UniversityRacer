#include "Scene.h"

using namespace std;


/**
 * Jeden vrchol ve VBO slozeny ze souradnic a souradnic textury
 */
typedef struct VBOENTRY {
	float x, y, z;
	float u, v;
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


	// !!! tmp pro testovani !!!
	glEnable(GL_LIGHT0);
	GLfloat lightPos[] = { 0, 2, 0, 1 }; // homogenni souradnice
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


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
		map<string, BaseModel*> models = container->getModels();
		
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
			for (map<string, BaseModel*>::iterator modelit = models.begin(); modelit != models.end(); modelit++)
			{
				BaseModel* model = (*modelit).second;

				for (vector<Mesh*>::iterator meshit = model->getMeshes().begin(); meshit != model->getMeshes().end(); meshit++)
				{
					Mesh* mesh = (*meshit);

					for (unsigned int i = 0; i < mesh->getVertices().size(); i++)
					{
						glm::vec3 vert = mesh->getVertices()[i];

						glm::vec2 tex(0, 0);
						if (mesh->getTexCoords().size() > i)
							tex = mesh->getTexCoords()[i];				

						// zapsat data a posunout ukazatel na nasledujici volne misto
						VBOENTRY e = {vert.x, vert.y, vert.z, tex.r, tex.s};
						*mapping = e;
						mapping++;

						//cout << "v[" << i << "]\t" << vert.x << "\t" << vert.y << "\t" << vert.z << endl;
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

			unsigned int modelIndexOffset = 0; // kolik indexu uz zapsaly predchozi modely
			unsigned int meshIndexOffset = 0; // kolik indexu uz zapsaly predchozi meshe tohoto modelu

			// naplneni daty
			for (map<string, BaseModel*>::iterator modelit = models.begin(); modelit != models.end(); modelit++)
			{
				BaseModel* model = (*modelit).second;
			
				for (unsigned int mi = 0; mi < model->getMeshes().size(); mi++)
				{
					Mesh* mesh = model->getMeshes()[mi];					

					for (unsigned int i = 0; i < mesh->getFaces().size(); i++)
					{
						glm::vec3 face = mesh->getFaces().at(i);

						// zapsat data a posunout ukazatel na nasledujici volne misto					
						mapping[writingIndex + 0] = (unsigned int)face.x + modelIndexOffset + meshIndexOffset;
						mapping[writingIndex + 1] = (unsigned int)face.y + modelIndexOffset + meshIndexOffset;
						mapping[writingIndex + 2] = (unsigned int)face.z + modelIndexOffset + meshIndexOffset;

						//cout << "i\t" << mapping[writingIndex + 0] << "\t" << mapping[writingIndex + 1] << "\t" << mapping[writingIndex + 2] << endl;

						writingIndex += 3;					
					}	
					
					// pricist pocet vrcholu teto meshe; pristi musi nalezite precislovat indexy
					meshIndexOffset += mesh->getVertices().size();
				}
				
				// pricist pocet vrcholu tohoto modelu
				modelIndexOffset += model->vertexCount();
				meshIndexOffset = 0;
			}

			// presun dat do graficke pameti
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}
		
	}	
}




void Scene::draw() 
{
	ShaderManager::MATERIAL mat = ShaderManager::useMaterial("default");
	
    // pohledova matice
	glm::mat4 mView = application.getCamera()->GetMatrix();
	glUniformMatrix4fv(mat.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));

	// projekcni matice
	glm::mat4 mProjection = glm::perspective(45.0f, (float)application.getWindowAspectRatio(), 1.0f, 1000.0f);
	glUniformMatrix4fv(mat.mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjection));

	// modelova matice - !!! tmp reseni !!!
	glm::mat4 mModel(1.0);
	glUniformMatrix4fv(mat.mModelUniform, 1, GL_FALSE, glm::value_ptr(mModel));

	// nastaveni svetel - !!! tmp reseni !!!
	GLint lights[] = {
		1, 0, 0, 0, 0, 0, 0, 0
	};
	GLuint enabledLightsUniform = glGetUniformLocation(ShaderManager::getCurrentMaterial().program, "enabledLights");
	glUniform1iv(enabledLightsUniform, 8, lights);


	for (unsigned int i = 0; i < containers.size(); i++)
	{
		glEnableVertexAttribArray(mat.positionAttrib);
		glEnableVertexAttribArray(mat.texposAttrib);
	
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);

		glVertexAttribPointer(mat.positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, x));
		glVertexAttribPointer(mat.texposAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VBOENTRY), (void*)offsetof(VBOENTRY, u));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);

		glDrawElements(GL_TRIANGLES, containers[i]->facesCount() * 3, GL_UNSIGNED_INT, NULL);	
		//glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, (void*)(10 * 3 * sizeof(unsigned int)));

		break;
	}

}