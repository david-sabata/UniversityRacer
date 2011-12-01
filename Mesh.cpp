#include <iostream>
#include "Mesh.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

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
	
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		// spocitat per-face normaly
		glm::vec3 normal = glm::cross(
			vertices[ (unsigned int)faces[i].y ] - vertices[ (unsigned int)faces[i].x ],
			vertices[ (unsigned int)faces[i].z ] - vertices[ (unsigned int)faces[i].x ]
		);		

		normals[ (unsigned int)faces[i].x ] += normal;
		normals[ (unsigned int)faces[i].y ] += normal;
		normals[ (unsigned int)faces[i].z ] += normal;


		// spocitat per-face tangenty
		unsigned int iv1 = faces[i].y;
		unsigned int iv2 = faces[i].x;

		glm::vec2 tc1(0, 0);
		if (texcoords.size() > iv1) tc1 = texcoords[iv1];

		glm::vec2 tc2(0, 0);
		if (texcoords.size() > iv1) tc2 = texcoords[iv2];

		float frac = tc1.x * tc2.y - tc2.x * tc1.y;
		float coef = 1;
		if (frac != 0)
			coef = 1 / frac;
		
		glm::vec3 tangent = glm::vec3(
			coef * ((vertices[iv1].x * tc2.x)  + (vertices[iv2].x * -tc1.y)),
			coef * ((vertices[iv1].y * tc2.x)  + (vertices[iv2].y * -tc1.y)),
			coef * ((vertices[iv1].z * tc2.x)  + (vertices[iv2].z * -tc1.y))
		);

		tangents[ faces[i].x ] += tangent;
		tangents[ faces[i].y ] += tangent;
		tangents[ faces[i].z ] += tangent;
	}

	// spocitat per-vertex normaly
	for (vector<glm::vec3>::iterator it = normals.begin(); it != normals.end(); it++)
	{
		(*it) = glm::normalize(*it);		
	}

	// spocitat per-vertex tangenty
	for (vector<glm::vec3>::iterator it = tangents.begin(); it != tangents.end(); it++)
	{
		(*it) = glm::normalize(*it);		
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

std::vector<glm::vec3> const &Mesh::getTangents()
{
	return tangents;
}

std::vector<glm::vec2> const &Mesh::getTexCoords()
{
	return texcoords;
}

std::string const &Mesh::getMaterialName()
{
	return materialName;
}
