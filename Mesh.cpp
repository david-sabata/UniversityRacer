
#include "Mesh.h"
#include <iostream>

using namespace std;



Mesh::Mesh(string name, string materialName, vector<glm::vec3> vertices, 
			vector<glm::ivec3> faces, vector<glm::vec2> texcoords) : 
	name(name), materialName(materialName), vertices(vertices), faces(faces),
		texcoords(texcoords)
{
	// vynulovat normaly a tangenty
	normals = vector<glm::vec3>(vertices.size(), glm::vec3(0.0f));
	tangents = vector<glm::vec3>(vertices.size(), glm::vec3(0.0f));
	normalsComputed = false;

#if 0
		cout << "Mesh '" << name.c_str() << "' made of '" << materialName.c_str() << "'" << endl;
		cout << "\tverts: " << vertices.size() << "\tfaces: " << faces.size() << "\ttexcoords: " << texcoords.size() << endl << endl;
#endif
}



/**
 * http://www.lighthouse3d.com/opengl/terrain/index.php3?normals
 */
void Mesh::computeTangentsAndNormals()
{
	if (normalsComputed)
		return;

	vector<glm::vec3> faceNormals(faces.size());

	// spocitat per-face normaly
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		faceNormals[i] = glm::cross(
			vertices[ (unsigned int)faces[i].y ] - vertices[ (unsigned int)faces[i].x ],
			vertices[ (unsigned int)faces[i].z ] - vertices[ (unsigned int)faces[i].x ]
		);
		faceNormals[i] = glm::normalize(faceNormals[i]);

		/*
		glm::vec3 v0 = vertices[ (unsigned int)faces[i].x ];
		glm::vec3 v1 = vertices[ (unsigned int)faces[i].y ];
		glm::vec3 v2 = vertices[ (unsigned int)faces[i].z ];
		cout << "vert 0: " << v0.x << "\t" << v0.y << "\t" << v0.z << endl;
		cout << "vert 1: " << v1.x << "\t" << v1.y << "\t" << v1.z << endl;
		cout << "vert 2: " << v2.x << "\t" << v2.y << "\t" << v2.z << endl;
		cout << "face normal: " << faceNormals[i].x << "\t" << faceNormals[i].y << "\t" << faceNormals[i].z << endl;
		*/

		// akumulovat normaly ve vrcholech
		normals[ (unsigned int)faces[i].x ] += faceNormals[i];
		normals[ (unsigned int)faces[i].y ] += faceNormals[i];
		normals[ (unsigned int)faces[i].z ] += faceNormals[i];
	}

	// spocitat per-vertex normaly
	for (vector<glm::vec3>::iterator it = normals.begin(); it != normals.end(); it++)
	{
		(*it) = glm::normalize(*it);
		//~ cout << (*it).x << "," << (*it).y << "," << (*it).z << endl;
	}

	normalsComputed = true;
}



std::string const &Mesh::getName() 
{
	return name;
}

std::vector<glm::ivec3> const &Mesh::getFaces()
{
	return faces;
}

std::vector<glm::vec3> const &Mesh::getVertices() 
{
	return vertices;
}

std::vector<glm::vec3> const &Mesh::getNormals() 
{
	return normals;
}

std::vector<glm::vec2> const &Mesh::getTexCoords()
{
	return texcoords;
}

std::string const &Mesh::getMaterialName()
{
	return materialName;
}
