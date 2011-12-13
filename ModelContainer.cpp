#include "ModelContainer.h"
#include <stdexcept>

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

#define TEX2_SUFFIX "_normal"
#define TEX3_SUFFIX "_height"
#define TEX_EXT ".bmp"

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

vector<ModelContainer::MESHDRAWINGQUEUEITEM> const &ModelContainer::getMeshDrawingQueue()
{
	return meshDrawingQueue;
}





void ModelContainer::optimizeDrawingQueue()
{
	/**
	 * [material] => [
	 *       [Mesh*] => index do kreslici fronty
	 *       [Mesh*] => ...
	 * ],
	 * [material] => ...
	 */	
	typedef map<string, vector<pair<Mesh*, unsigned int>>> MATERIALMAP;

	// asociativni pole materialu ve scene a meshi (s vazbou na puvodni model) ktere je pouzivaji
	MATERIALMAP materials;

	// rozskatulkovat meshe podle pouziteho materialu
	for (unsigned int i = 0; i < drawingQueue.size(); i++)
	{
		BaseModel* model = drawingQueue[i].model;		

		for (vector<Mesh*>::iterator meshIt = model->getMeshes().begin(); meshIt != model->getMeshes().end(); meshIt++)
		{
			Mesh* mesh = (*meshIt);
			
			// najit material
			MATERIALMAP::iterator matIt = materials.find( mesh->getMaterialName() );
			
			// pokud material neexistuje, vyrobit novy zaznam
			if (matIt == materials.end()) {
				// vlozi prazdny zaznam a vraci jeho iterator
				matIt = materials.insert( 
							pair<string, vector<pair<Mesh*, unsigned int>>>( mesh->getMaterialName(), vector<pair<Mesh*, unsigned int>>() ) 
						).first;
			}

			// pridat mesh do spravne 'skatulky' a zapamatovat si index puvodni polozky
			(*matIt).second.push_back(pair<Mesh*, unsigned int>(mesh, i));
		}
	}
	
	// prochazet postupne materialy
	for (MATERIALMAP::iterator matIt = materials.begin(); matIt != materials.end(); matIt++)
	{
		// a jimi kreslene meshe
		for (vector<pair<Mesh*, unsigned int>>::iterator meshIt = (*matIt).second.begin(); meshIt != (*matIt).second.end(); meshIt++)
		{
			Mesh* mesh = (*meshIt).first;
			unsigned int drawingQueueIndex = (*meshIt).second;

			// pridat do kreslici fronty meshi
			MESHDRAWINGQUEUEITEM item = { mesh, drawingQueue[drawingQueueIndex].matrix };
			meshDrawingQueue.push_back(item);

			// najit model v mapovani
			map<unsigned int, vector<unsigned int>>::iterator mappingIt = drawingQueuesMapping.find(drawingQueueIndex);
			
			// vytvorit zaznam pokud neni
			if (mappingIt == drawingQueuesMapping.end()) {
				mappingIt = drawingQueuesMapping.insert( pair<unsigned int, vector<unsigned int>>( drawingQueueIndex, vector<unsigned int>() ) ).first;
			}

			// zapamatovat si, kteremu modelu tato mesh patrila (pro naslednou upravu matic) - index do meshDrawingQueue
			(*mappingIt).second.push_back( meshDrawingQueue.size() - 1 );
		}		
	}
}








void ModelContainer::addModel(string name, BaseModel *model) 
{
	// pridame model
	models.push_back(model);
	modelNames.insert( pair<string, unsigned int>(name, models.size()) );

	// pridame info o offsetu jeho indexu
	modelsIndexOffsets.insert(pair<BaseModel*, unsigned int>(model, totalFacesCount * 3));
	
	// info o indexech meshi
	unsigned int offset = totalFacesCount * 3;
	for (vector<Mesh*>::iterator it = model->getMeshes().begin(); it != model->getMeshes().end(); it++)
	{
		meshesIndexOffsets.insert(pair<Mesh*, unsigned int>((*it), offset));
		offset += (*it)->getFaces().size() * 3;
	}

	// a aktualizujeme pocet facu v kontejneru
	totalFacesCount += model->facesCount();
}






unsigned int ModelContainer::queueDraw(string modelName)
{
	return queueDraw(modelName, glm::mat4(1.0));
}

unsigned int ModelContainer::queueDraw(string modelName, glm::mat4 mat)
{
	map<string, unsigned int>::iterator it = modelNames.find(modelName);
	if (it == modelNames.end())
		runtime_error("Model queued for drawing has not been loaded into container");

	return queueDraw(models.at((*it).second), mat);
}

unsigned int ModelContainer::queueDraw(BaseModel* model)
{
	return queueDraw(model, glm::mat4(1.0));
}

unsigned int ModelContainer::queueDraw(BaseModel* model, glm::mat4 mat)
{
	// zaradit do modelove fronty
	DRAWINGQUEUEITEM item = {model, mat};
	drawingQueue.push_back(item);
	
	return drawingQueue.size() - 1;
}




void ModelContainer::updateDrawingMatrix(unsigned int drawingQueueIndex, glm::mat4 matrix)
{
	// najit indexy ve fronte meshi
	vector<unsigned int> meshIndices = drawingQueuesMapping[drawingQueueIndex];

	// aktualizovat vsem matice
	for (vector<unsigned int>::iterator it = meshIndices.begin(); it != meshIndices.end(); it++)
	{
		meshDrawingQueue[(*it)].matrix = matrix;
	}
}




unsigned int ModelContainer::getModelIndexOffset(BaseModel* model)
{
	return modelsIndexOffsets[model];
}


unsigned int ModelContainer::getMeshIndexOffset(Mesh* mesh)
{
	return meshesIndexOffsets[mesh];
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






void ModelContainer::addLight(Light light)
{
	lights.push_back(light);
}


vector<Light> &ModelContainer::getLights()
{
	return lights;
}








bool fileExists(string filename)
{
	ifstream ifile(filename);
	return (bool)ifile;
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

		// nazev materialu (jen prvni, pokud je vice)
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
		vector<glm::ivec3> faces;
		for (unsigned int i = 0; i < mesh.Faces().size(); i++)
		{
			Mesh3DSFace face = mesh.Faces()[i];
			faces.push_back( glm::ivec3(face.a, face.b, face.c) );
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


	// ulozit materialy
	vector<Material3DSObject> materials = scene->Materials();
	for (vector<Material3DSObject>::iterator it = materials.begin(); it != materials.end(); it++) {
		Material3DSObject m = (*it);

		ShaderManager::MATERIALPARAMS params;
		params.ambient = glm::vec4( m.AmbientColor().r, m.AmbientColor().g, m.AmbientColor().b, 1.0f );
		params.diffuse = glm::vec4( m.DiffuseColor().r, m.DiffuseColor().g, m.DiffuseColor().b, 1.0f );
		params.specular = glm::vec4( m.SpecularColor().r, m.SpecularColor().g, m.SpecularColor().b, 1.0f );
		params.shininess = m.Shininess();
		
		// nacist textury do GL, pokud odpovidajici soubory existuji
		// hleda se automaticky zakladni textura, normalova a vyskova
		string path = ShaderManager::getTexturesPath();
		if (fileExists(path + m.Texture())) {
			params.textures.push_back( ShaderManager::loadTexture(path + m.Texture()) );

			string::size_type pos = m.Texture().find_last_of(".");
			string baseName = m.Texture().substr(0, pos);

			string normalTexture = path + baseName + TEX2_SUFFIX + TEX_EXT;
			if (fileExists(normalTexture))
				params.textures.push_back( ShaderManager::loadTexture(normalTexture) );

			string heightTexture = path + baseName + TEX3_SUFFIX + TEX_EXT;
			if (fileExists(heightTexture))
				params.textures.push_back( ShaderManager::loadTexture(heightTexture) );
		}

		// za shader povazujeme vse pred podtrzitkem, pokud se v nazvu vyskytuje
		string::size_type pos = m.Name().find("_");
		if (pos != string::npos)
			ShaderManager::addMaterial(m.Name().substr(0, pos));
		else
			ShaderManager::addMaterial(m.Name());

		// priradit parametry materialu vsem meshim ktere ho pouzivaji
		for (vector<Mesh*>::iterator meshIt = modelMeshes.begin(); meshIt != modelMeshes.end(); meshIt++)
		{
			if ((*meshIt)->getMaterialName() == m.Name())
				(*meshIt)->setMaterialParams(params);
		}

#if 0
		cout << "--- in " << filename << " ---------------------------------------------" << endl;
		cout << "Material: " << m.Name() << endl;
		cout << "Ambient: " << m.AmbientColor().r << " / " << m.AmbientColor().g << " / " << m.AmbientColor().b << endl;
		cout << "Diffuse: " << m.DiffuseColor().r << " / " << m.DiffuseColor().g << " / " << m.DiffuseColor().b << endl;
		cout << "Specular: " << m.SpecularColor().r << " / " << m.SpecularColor().g << " / " << m.SpecularColor().b << endl;
		cout << "Shininess percentage: " << m.Shininess() << endl;
		cout << "Texture: " << m.Texture() << endl;
#endif
	}

	// ulozit svetla
	vector<Light3DSObject> parsedLights = scene->Lights();
	for (vector<Light3DSObject>::iterator it = parsedLights.begin(); it != parsedLights.end(); it++) {
		
		// vse v 3DS je Z-up; transformovat na Y-up
		Light3DSObject::POSITION pos = (*it).Position();
		glm::vec4 glmvert(pos.x, pos.y, pos.z, 1);				
		glm::mat4 rotate = glm::rotate(glm::mat4(), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glmvert = glmvert * rotate;

		// TMP: scalovat svetlo stejne jako se bude scalovat ucebna
		// TODO: globalni matice kontejneru ktera ovlivni i svetla
		glm::mat4 modelmat = glm::scale(glm::mat4(), glm::vec3(0.05));
		glmvert = glmvert * modelmat;

		// pridat svetla s defaultnimi barevnymi vlastnostmi (bile)
		Light l = Light(glmvert);		
		addLight(l);
	}
	
	delete scene;

	// ulozit a vratit model
	model->setMeshes(modelMeshes);
	return model;
}
