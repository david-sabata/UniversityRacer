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
 * Konstruktor pro mesh jejiz vsechny vrcholy se ihned po vytvoreni 
 * vynasobi danou matici
 */
Mesh::Mesh(const Mesh& mesh, const glm::mat4& matrix)
{
	name = mesh.getName();
	materialName = mesh.getMaterialName();
	faces = mesh.getFaces();
	texcoords = mesh.getTexCoords();
	
	vector<glm::vec3> meshVerts = mesh.getVertices();
	vertices = vector<glm::vec3>(meshVerts.size());

	for (unsigned int i = 0; i < meshVerts.size(); i++)
	{
		glm::vec4 vert = glm::vec4(meshVerts[i], 1.0f);
		glm::vec4 multed = matrix * vert;
		vertices[i] = glm::vec3(multed);
	}

	normals = vector<glm::vec3>(vertices.size(), glm::vec3(0.0f));
	tangents = vector<glm::vec3>(vertices.size(), glm::vec3(0.0f));

	if (mesh.areNormalsComputed()) {
		normalsComputed = true;

		vector<glm::vec3> meshNormals = mesh.getNormals();
		for (unsigned int i = 0; i < meshNormals.size(); i++)
		{
			normals[i] = glm::vec3(glm::vec4(meshNormals[i], 0.0f) * matrix);
		}

		vector<glm::vec3> meshTangents = mesh.getTangents();
		for (unsigned int i = 0; i < meshTangents.size(); i++)
		{
			tangents[i] = glm::vec3(glm::vec4(meshTangents[i], 0.0f) * matrix);
		}
	}
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
		glm::vec3 v1 = vertices[ (unsigned int)faces[i].x ];
		glm::vec3 v2 = vertices[ (unsigned int)faces[i].y ];
		glm::vec3 v3 = vertices[ (unsigned int)faces[i].z ];

		// texturovaci souradnice nemusi existovat
		glm::vec2 tc1 = texcoords.size() > (unsigned int)faces[i].x ? texcoords[ faces[i].x ] : glm::vec2(0, 0);
		glm::vec2 tc2 = texcoords.size() > (unsigned int)faces[i].y ? texcoords[ faces[i].y ] : glm::vec2(0, 0);
		glm::vec2 tc3 = texcoords.size() > (unsigned int)faces[i].z ? texcoords[ faces[i].z ] : glm::vec2(0, 0);

		// spocitat per-face normaly
		glm::vec3 normal = glm::cross(
			v2 - v1,
			v3 - v1
		);		

		normals[ (unsigned int)faces[i].x ] += normal;
		normals[ (unsigned int)faces[i].y ] += normal;
		normals[ (unsigned int)faces[i].z ] += normal;

		
		// Vypocet tangent pro parallax mapping

		// http://www.dhpoware.com/demos/glslParallaxNormalMapping.html
		// Given the 3 vertices (position and texture coordinates) of a triangle
		// calculate and return the triangle's tangent vector.

		// Create 2 vectors in object space.
		//
		// edge1 is the vector from vertex positions pos1 to pos2.
		// edge2 is the vector from vertex positions pos1 to pos3.
		glm::vec3 edge1 = glm::normalize(glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z));
		glm::vec3 edge2 = glm::normalize(glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z));
		
		// Create 2 vectors in tangent (texture) space that point in the same
		// direction as edge1 and edge2 (in object space).
		//
		// texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
		// texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
		glm::vec2 texEdge1 = glm::normalize(glm::vec2(tc2.x - tc1.x, tc2.y - tc1.y));
		glm::vec2 texEdge2 = glm::normalize(glm::vec2(tc3.x - tc1.x, tc3.y - tc1.y));

		// These 2 sets of vectors form the following system of equations:
		//
		//  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
		//  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
		//
		// Using matrix notation this system looks like:
		//
		//  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
		//  [       ]  =  [                        ]  [           ]
		//  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
		//
		// The solution is:
		//
		//  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
		//  [           ]  =  -------  [                         ]  [       ]
		//  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
		//
		//  where:
		//        [ texEdge1.x  texEdge1.y ]
		//    A = [                        ]
		//        [ texEdge2.x  texEdge2.y ]
		//
		//    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
		//
		// From this solution the tangent space basis vectors are:
		//
		//    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
		//  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
		//     normal = cross(tangent, bitangent)

		glm::vec3 tangent;
		
		float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

		if (det < FLT_EPSILON)
		{
			tangent = glm::vec3(1.0f, 0.0f, 0.0f);			
		}
		else
		{
			det = 1.0f / det;

			tangent.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
			tangent.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
			tangent.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;
			
			tangent = glm::normalize(tangent);
		}

		tangents[ faces[i].x ] = tangent;
		tangents[ faces[i].y ] = tangent;
		tangents[ faces[i].z ] = tangent;
	}

	// spocitat per-vertex normaly
	for (vector<glm::vec3>::iterator it = normals.begin(); it != normals.end(); it++)
	{
		(*it) = glm::normalize(*it);		
	}

	normalsComputed = true;
}




std::string const &Mesh::getName() const
{
	return name;
}

std::vector<glm::ivec3> const &Mesh::getFaces() const
{
	return faces;
}

std::vector<glm::vec3> const &Mesh::getVertices() const
{
	return vertices;
}

std::vector<glm::vec3> const &Mesh::getNormals() const
{
	return normals;
}

std::vector<glm::vec3> const &Mesh::getTangents() const
{
	return tangents;
}

std::vector<glm::vec2> const &Mesh::getTexCoords() const
{
	return texcoords;
}

std::string const &Mesh::getMaterialName() const
{
	return materialName;
}

bool Mesh::areNormalsComputed() const
{
	return normalsComputed;
}

void Mesh::setMaterialParams(ShaderManager::MATERIALPARAMS params)
{
	material = params;
}

ShaderManager::MATERIALPARAMS const& Mesh::getMaterialParams() const
{
	return material;
}

Mesh Mesh::operator* (glm::mat4 const& matrix) const
{
	const Mesh& self = *this;
	return Mesh(self, matrix);
}