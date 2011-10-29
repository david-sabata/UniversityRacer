#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>

#include <GL/glew.h>

#include "Exceptions.h"


class ShaderManager
{
	public:
		/**
		 * Struktura materialu; obsahuje handles GL objektu shaderu, aby
		 * nebylo nutne je ziskavat stale dokola z GL
		 */
		typedef struct Material {
			GLuint program;
			GLuint positionAttrib, texposAttrib;
			GLuint mViewUniform;
			GLuint mProjectionUniform;
			GLuint mModelUniform;
		} MATERIAL;

		/**
		 * Nacte material podle jeho nazvu
		 */
		static void loadMaterial(std::string material);

		/**
		 * Nastavi material jako aktualni pro kresleni
		 * a soucasne vraci jeho strukturu
		 */
		static MATERIAL useMaterial(std::string material);

		/**
		 * Vraci strukturu aktualniho materialu
		 */
		static MATERIAL getCurrentMaterial();		
		
		// docasne public pro moznost vypsat materialy z Game.cpp
		static std::map<std::string, MATERIAL> materials;

	protected:
		/**
		 * Aktualne pouzivany material
		 */
		static MATERIAL currentMaterial;
		

		///////////////////////////////////////////////////////////////
		// Funkce ze cviceni
		///////////////////////////////////////////////////////////////

		// Load whole file and return it as std::string
		static std::string loadFile(const char * const file);

		// Info log contains errors and warnings from shader compilation
		static std::string getShaderInfoLog(const GLuint shader);

		// Info log contains errors and warnings from shader linking
		static std::string getProgramInfoLog(const GLuint program);

		static GLuint compileShader(const GLenum type, const char * source);
		static GLuint linkShader(size_t count, ...);

};


#endif