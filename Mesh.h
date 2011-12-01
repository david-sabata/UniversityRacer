#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include "Debug.h"


class Mesh
{	

	public:
		
		Mesh(std::string name, std::string materialName, std::vector<glm::vec3> vertices, 
			std::vector<glm::ivec3> faces, std::vector<glm::vec2> texcoords);
		
		/////////////////////////////////////////////////////////////////////////

		/**
		 * Spocita per-vertex tangenty a normaly
		 */
		void computeTangentsAndNormals();


		/**
		 * Vraci nazev meshe
		 */
		std::string const &getName();

		/**
		 * Vraci referenci na pole trojuhelniku. Kazdy bod
		 * pole slouzi jako index do pole vrcholu
		 */
		std::vector<glm::ivec3> const &getFaces();

		/**
		 * Vraci referenci na pole vrcholu modelu
		 */
		std::vector<glm::vec3> const& getVertices();

		/**
		 * Vraci referenci na pole normal odpovidajicich vrcholum
		 */
		std::vector<glm::vec3> const& getNormals();

		/**
		 * Vraci referenci na pole tangent odpovidajici vrcholum
		 */
		std::vector<glm::vec3> const& getTangents();

		/**
		 * Vraci referenci na pole souradnic textur. Indexy
		 * odpovidaji indexum do pole vrcholu
		 */
		std::vector<glm::vec2> const& getTexCoords();

		/**
		 * Vraci referenci na nazev materialu
		 */
		std::string const& getMaterialName();
		

	protected:
		std::vector<glm::ivec3> faces;

		std::vector<glm::vec3> vertices;

		std::vector<glm::vec3> normals;

		std::vector<glm::vec3> tangents;

		std::vector<glm::vec2> texcoords;

		std::string name;

		std::string materialName;

		bool normalsComputed;

};

#endif
