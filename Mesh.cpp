
#include "Mesh.h"
#include <iostream>

using namespace std;



Mesh::Mesh(string name, string materialName, vector<glm::vec3> vertices, 
			vector<glm::vec3> faces, vector<glm::vec2> texcoords) : 
	name(name), materialName(materialName), vertices(vertices), faces(faces),
		texcoords(texcoords)
{
	cout << "Mesh '" << name.c_str() << "' made of '" << materialName.c_str() << "'" << endl;
	cout << "\tverts: " << vertices.size() << "\tfaces: " << faces.size() << "\ttexcoords: " << texcoords.size() << endl << endl;
}


std::string const &Mesh::getName() 
{
	return name;
}

std::vector<glm::vec3> const &Mesh::getFaces()
{
	return faces;
}

std::vector<glm::vec3> const &Mesh::getVertices() 
{
	return vertices;
}

std::vector<glm::vec2> const &Mesh::getTexCoords()
{
	return texcoords;
}

std::string const &Mesh::getMaterialName()
{
	return materialName;
}