#ifndef LIGHT_H
#define LIGHT_H

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif


#define DEFAULT_AMBIENT glm::vec4(0.1, 0.1, 0.1, 1)
#define DEFAULT_DIFFUSE glm::vec4(1, 1, 1, 1)



class Light {
	public:
		
		/**
		 * Konstruktor s moznosti nastavit vlastni barevne slozky
		 */
		inline Light(glm::vec4 position, glm::vec4 diffuse = DEFAULT_DIFFUSE, glm::vec4 ambient = DEFAULT_AMBIENT) :
					position(position), diffuse(diffuse), ambient(ambient) {};

		/**
		 * Gettery jednotlivych vlastnosti svetla
		 */
		inline const glm::vec4 &Position()
			{ return position; }

		inline const glm::vec4 &Diffuse()
			{ return diffuse; }

		inline const glm::vec4 &Ambient()
			{ return ambient; }

	protected:
		glm::vec4 position;
		glm::vec4 diffuse;
		glm::vec4 ambient;
};


#endif