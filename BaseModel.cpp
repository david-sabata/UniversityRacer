
#include "BaseModel.h"
#include <iostream>
#include <string>

using namespace std;



BaseModel::~BaseModel() 
{
	// uvolnit z pameti vsechny meshe
	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		delete (*it);
	}
}



void BaseModel::addMesh(Mesh* mesh)
{
	meshes.push_back(mesh);	
}



vector<Mesh*> &BaseModel::getMeshes()
{
	return meshes;
}


unsigned int BaseModel::vertexCount()
{
	unsigned int total = 0;

	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
		total += (*it)->getVertices().size();

	return total;
}


unsigned int BaseModel::facesCount()
{
	unsigned int total = 0;

	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
		total += (*it)->getFaces().size();

	return total;
}