#include "CachedModel.h"

using namespace std;


CachedModel::CachedModel(BaseModel* model, string path)
{
	ofstream f (path.c_str(), ofstream::binary);

	for (vector<Mesh*>::iterator it = model->getMeshes().begin(); it != model->getMeshes().end(); it++)
	{
		Mesh* mesh = (*it);
		if (mesh->getVertices().size() > 0)
			f.write((char*)mesh, sizeof(Mesh));
	}

	f.close();

	meshes = vector<Mesh*>(model->getMeshes());
}






CachedModel::CachedModel(vector<Mesh*> loadedMeshes)
{
	meshes = loadedMeshes;
}




CachedModel* CachedModel::load(string path)
{
	ifstream f (path.c_str(), ifstream::binary);
	if (!f.good())
		return NULL;

	
	Mesh* mesh = (Mesh*) malloc(sizeof(Mesh));

	while (f.good()) {
		f.read((char*)mesh, sizeof(Mesh));
		cout << mesh->getName() << endl;
		cout << "verts: " << mesh->getVertices().size() << endl;
	}	

	free(mesh);
	f.close();
	return NULL;
}