
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



void BaseModel::setMeshes(vector<Mesh*> meshes)
{
	this->meshes = meshes;

	vertexCount = 0;
	faceCount = 0;

	// spocitat vrcholy a facy
	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		vertexCount += (*it)->getVertices().size();
		faceCount += (*it)->getFaces().size();
	}
}



vector<Mesh*> &BaseModel::getMeshes()
{
	return meshes;
}
