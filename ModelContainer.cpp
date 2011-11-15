#include "ModelContainer.h"

using namespace std;


ModelContainer::ModelContainer(void)
{
	totalFacesCount = 0;
}


ModelContainer::~ModelContainer(void)
{
	// uvolnit vsechny modely
	for (vector<BaseModel*>::iterator it = models.begin(); it != models.end(); it++)
	{
		delete (*it);
	}
}







vector<BaseModel*> const &ModelContainer::getModels()
{
	return models;
}


vector<ModelContainer::DRAWINGQUEUEITEM> const &ModelContainer::getDrawingQueue()
{
	return drawingQueue;
}








void ModelContainer::addModel(string name, BaseModel *model) 
{
	// pridame model
	models.push_back(model);
	modelNames.insert( pair<string, unsigned int>(name, models.size()) );

	// pridame info o offsetu jeho indexu
	modelsIndexOffsets.insert(pair<BaseModel*, unsigned int>(model, totalFacesCount * 3));
	
	// a aktualizujeme pocet facu v kontejneru
	totalFacesCount += model->facesCount();
}






void ModelContainer::queueDraw(string modelName)
{
	queueDraw(modelName, glm::mat4(1.0));
}

void ModelContainer::queueDraw(string modelName, glm::mat4 mat)
{
	map<string, unsigned int>::iterator it = modelNames.find(modelName);
	if (it == modelNames.end())
		runtime_error("Model queued for drawing has not been loaded into container");

	queueDraw(models.at((*it).second), mat);
}

void ModelContainer::queueDraw(BaseModel* model)
{
	queueDraw(model, glm::mat4(1.0));
}

void ModelContainer::queueDraw(BaseModel* model, glm::mat4 mat)
{
	DRAWINGQUEUEITEM item = {model, mat};
	drawingQueue.push_back(item);
}







unsigned int ModelContainer::getModelIndexOffset(BaseModel* model)
{
	return modelsIndexOffsets[model];
}






unsigned int ModelContainer::verticesCount()
{
	unsigned int total = 0;

	for (vector<BaseModel*>::iterator it = models.begin(); it != models.end(); it++)
	{
		for (vector<Mesh*>::iterator mit = (*it)->getMeshes().begin(); mit != (*it)->getMeshes().end(); mit++)
		{
			total += (*mit)->getVertices().size();
		}				
	}

	return total;
}



unsigned int ModelContainer::facesCount()
{
	return totalFacesCount;
}


unsigned int ModelContainer::modelsCount()
{
	return models.size();
}









BaseModel* ModelContainer::load3DS(string filename) 
{
	// parsovani souboru
	Scene3DS *scene = new Scene3DS(filename.c_str());
	vector<Mesh3DSObject> meshes = scene->Meshes();

	// vytvorime novy model
	BaseModel* model = new BaseModel;
	vector<Mesh*> modelMeshes;

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
		
		if (faces.size() > 0) {
			Mesh* m = new Mesh(name, material, vertices, faces, texcoords);
			modelMeshes.push_back(m);
		}
	}
	
	model->setMeshes(modelMeshes);

	delete scene;

	//addModel(filename, model);
	return model;
}


