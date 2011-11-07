#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include <SDL/SDL.h>

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Exceptions.h"


class ShaderManager
{
	public:
		
		/**
		 * Vazba objektu textury na konkretni uniformy shaderu
		 */
		typedef struct TextureBinding {
			// GL handle uniformniho vstupu shaderu
			GLuint uniform;

			// GL handle pouzite textury
			GLuint texture;

			// GL parametry textury
			GLint MIN_FILTER;
			GLint MAG_FILTER;
			GLint WRAP_S;
			GLint WRAP_T;

			// binding je platny, tj. jeho textura je nactena v GL
			bool isValid;
		} TEXTUREBINDING;
		

		/**
		 * Struktura materialu; obsahuje handles GL objektu shaderu, aby
		 * nebylo nutne je ziskavat stale dokola z GL
		 */
		typedef struct Material {
			// vyuziva ShaderManager ----------
			GLuint program;
			std::vector<TEXTUREBINDING> textures;
			// --------------------------------

			// vyuziva vykreslujici scena -----
			GLuint positionAttrib, normalAttrib, texposAttrib;
			GLuint mViewUniform;
			GLuint mProjectionUniform;
			GLuint mModelUniform;
			// --------------------------------
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
		
	protected:
		/**
		 * Aktualne pouzivany material
		 */
		static MATERIAL currentMaterial;

		/**
		 * Nazvy materialu a jim odpovidajici struktury
		 */
		static std::map<std::string, MATERIAL> materials;

		/**
		 * Nazvy souboru textur a jim odpovidajici GL objekty
		 */
		static std::map<std::string, GLuint> textures;
		
		///////////////////////////////////////////////////////////////
		
		/**
		 * Prohleda zdrojovy kod shaderu a dohleda reference na textury,
		 * ktere nacte a vrati odpovidajici struktury materialu
		 * 
		 * FEATURE: idealni by bylo oddelit prohledani shaderu do 'annotationParseru'
		 *			a semantiku anotaci resit az nasledne
		 */
		static std::vector<TEXTUREBINDING> loadTextures(GLuint program, std::string source);




		///////////////////////////////////////////////////////////////
		// Funkce ze cviceni
		///////////////////////////////////////////////////////////////

		// Nacte obrazek do textury
		static void SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface);

		// Load whole file and return it as std::string
		static std::string loadFile(const char * const file);

		// Info log contains errors and warnings from shader compilation
		static std::string getShaderInfoLog(const GLuint shader);

		// Info log contains errors and warnings from shader linking
		static std::string getProgramInfoLog(const GLuint program);

		static GLuint compileShader(const GLenum type, const char * source);
		static GLuint linkShader(size_t count, ...);

		// vertikalne prevrati obrazek (texturu)
		static void verticalSurfaceFlip(SDL_Surface*& image);
};


#endif
