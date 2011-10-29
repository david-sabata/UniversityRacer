#ifndef MESH_H
#define MESH_H

#include <vector>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif


class Mesh
{	

	public:
		
		Mesh(std::string name, std::string materialName, std::vector<glm::vec3> vertices, 
			std::vector<glm::vec3> faces, std::vector<glm::vec2> texcoords);
		
		/////////////////////////////////////////////////////////////////////////


		/**
		 * Vraci nazev meshe
		 */
		std::string const &getName();

		/**
		 * Vraci ukazatel pole trojuhelniku. Kazdy bod
		 * pole slouzi jako index do pole vrcholu
		 */
		std::vector<glm::vec3> const &getFaces();

		/**
		 * Vraci ukazatel na pole vrcholu modelu
		 */
		std::vector<glm::vec3> const& getVertices();

		/**
		 * Vraci ukazatel na pole souradnic textur. Indexy
		 * odpovidaji indexum do pole vrcholu
		 */
		std::vector<glm::vec2> const& getTexCoords();

		/**
		 * Vraci retezec identifikujici material
		 */
		std::string const& getMaterialName();
		

	protected:
		std::vector<glm::vec3> faces;

		std::vector<glm::vec3> vertices;

		std::vector<glm::vec2> texcoords;

		std::string name;

		std::string materialName;

};

#endif