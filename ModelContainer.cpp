#include "ModelContainer.h"

using namespace std;


ModelContainer::ModelContainer(void)
{
}


ModelContainer::~ModelContainer(void)
{
	// uvolnit vsechny modely
	for (map<string, BaseModel*>::iterator it = models.begin(); it != models.end(); it++)
	{
		delete (*it).second;
	}
}



map<string, BaseModel*> const &ModelContainer::getModels()
{
	return models;
}



void ModelContainer::addModel(string name, BaseModel *model) 
{
	models.insert( pair<string, BaseModel*>(name, model) );
}




unsigned int ModelContainer::verticesCount()
{
	unsigned int total = 0;

	for (map<string, BaseModel*>::iterator it = models.begin(); it != models.end(); it++)
	{
		for (vector<Mesh*>::iterator mit = (*it).second->getMeshes().begin(); mit != (*it).second->getMeshes().end(); mit++)
		{
			total += (*mit)->getVertices().size();
		}				
	}

	return total;
}




unsigned int ModelContainer::facesCount()
{
	unsigned int total = 0;

	for (map<string, BaseModel*>::iterator it = models.begin(); it != models.end(); it++)
	{
		for (vector<Mesh*>::iterator mit = (*it).second->getMeshes().begin(); mit != (*it).second->getMeshes().end(); mit++)
		{
			total += (*mit)->getFaces().size();
		}				
	}

	return total;
}


unsigned int ModelContainer::modelsCount()
{
	return models.size();
}



void ModelContainer::load3DS(string filename) 
{
	// parsovani souboru
	Scene3DS *scene = new Scene3DS(filename.c_str());
	vector<Mesh3DSObject> meshes = scene->Meshes();

	// vytvorime novy model
	BaseModel* model = new BaseModel;
	

	for (vector<Mesh3DSObject>::iterator it = meshes.begin(); it != meshes.end(); it++) 
	{
		Mesh3DSObject mesh = (*it);

		// nazev
		string name = mesh.Name();

		// material (jen prvni, pokud je vice)
		string material = "";
		if (mesh.Materials().size() > 0)
		{
			material = (*mesh.Materials().begin()).first;
		}

		// vrcholy
		vector<glm::vec3> vertices;
		for (unsigned int i = 0; i < mesh.Vertices().size(); i++)
		{
			Mesh3DSVertex vert = mesh.Vertices()[i];
			glm::vec4 glmvert(vert.x, vert.y, vert.z, 1);

			// 3DS modely jsou Z-up; transformovat na Y-up
			glm::mat4 rotate = glm::rotate(glm::mat4(), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			glmvert = glmvert * rotate;

			vertices.push_back(glm::vec3(glmvert));
		}

		// indexy
		vector<glm::vec3> faces;
		for (unsigned int i = 0; i < mesh.Faces().size(); i++)
		{
			Mesh3DSFace face = mesh.Faces()[i];
			faces.push_back( glm::vec3(face.a, face.b, face.c) );
		}

		// texturovaci souradnice
		vector<glm::vec2> texcoords;
		for (unsigned int i = 0; i < mesh.TextureCoords().size(); i++)
		{
			Mesh3DSTextureCoord coord = mesh.TextureCoords()[i];
			texcoords.push_back( glm::vec2(coord.u, coord.v) );
		}
		

		Mesh* m = new Mesh(name, material, vertices, faces, texcoords);
		model->addMesh(m);
	}


	addModel(filename, model);

	delete scene;
}


