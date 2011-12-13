#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <SDL/SDL.h>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Debug.h"
#include "Exceptions.h"


/**
 * Trida zodpovedna za spravu shaderu
 *
 *
 * TODO: Pro budouci vaznejsi pouziti by bylo vhodne hloubeji promyslet provazani
 * shaderu a parametru materialu ziskanych primo z modelu, resp. z modelovaciho sw.
 * Idealem by bylo pouze nekolik typovych shaderu (lesk, mat,..) + nastavovani barev
 * atp. pomoci parametru; neni ovsem jasne, zda jde s 3DS toto provest.
 */
class ShaderManager
{
	public:
		
		/**
		 * Vazba objektu textury na konkretni uniformy shaderu
		 */
		typedef struct TextureBinding {
			// GL handle uniformniho vstupu shaderu
			GLint uniform;

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
		 * Struktura programu shaderu; obsahuje handles GL objektu shaderu, aby
		 * nebylo nutne je ziskavat stale dokola z GL
		 */
		typedef struct Program {
			// vyuziva ShaderManager ----------
			GLuint program;
			//std::vector<TEXTUREBINDING> textures;
			// --------------------------------

			// vyuziva vykreslujici scena -----
			GLuint positionAttrib, normalAttrib, tangentAttrib, texposAttrib;
			GLuint mViewUniform;
			GLuint mModelViewUniform;
			GLuint mModelViewProjectionUniform;
			GLuint mMVInverseTranspose;
			GLuint iEnabledLightsUniform;
			GLuint vLightsUniform;
			GLuint bDrawAmbientUniform;
			GLuint bDrawDiffSpecUniform;
			GLuint bUseTextureUniform;
			std::vector<GLuint> textureUniforms;
			struct {
				GLuint ambient;
				GLuint diffuse;
				GLuint specular;
				int shininess;
			} matParams;
			// --------------------------------
		} PROGRAMBINDING;


		/**
		 * Konkretni vlastnosti materialu
		 */
		typedef struct MaterialParams {
			glm::vec4 ambient;
			glm::vec4 diffuse;
			glm::vec4 specular;
			int shininess;
			std::vector<GLuint> textures;
		} MATERIALPARAMS;


		/**
		 * Nacte programy odpovidajici pouzitym materialum
		 */
		static void loadPrograms();

		/**
		 * Nacte program odpovidajici materialu (ale primo jej nepouzije)
		 */
		static bool loadProgram(std::string material);

		/**
		 * Prida material pouzivany ve scene (bude se nacitat jeho shader)
		 */
		inline static void addMaterial(std::string material)
		{
			if (find(shadersToLoad.begin(), shadersToLoad.end(), material) == shadersToLoad.end())
				shadersToLoad.push_back(material);
		}

		/**
		 * Asociuje konkretni hodnoty s promennymi programu
		 */
		static void setMaterialParams(std::string material, MATERIALPARAMS params);

		/**
		 * Nastavi program jako aktualni pro kresleni
		 * a soucasne vraci jeho strukturu
		 */
		static PROGRAMBINDING useProgram(std::string material);

		/**
		 * Nastavi aktualnimu materialu parametry
		 */
		static void useMaterialParams(MATERIALPARAMS params);

		/**
		 * Vraci strukturu aktualniho shaderu
		 */
		static PROGRAMBINDING getCurrentProgram();
		
		/**
		 * Vraci vychozi parametry materialu, pokud by zadne jine nebyly
		 */
		static MATERIALPARAMS getDefaultMaterial();

		/**
		 * Nacte texturu ze zadane cesty do GL a vraci jeji handle
		 */
		static GLuint loadTexture(std::string filename);

		/**
		 * Vraci cestu ve ktere se budou hledat textury
		 */
		static std::string ShaderManager::getTexturesPath();
		
	protected:
		/**
		 * Aktualne pouzivany material
		 */
		static PROGRAMBINDING currentProgram;

		/**
		 * Nazvy shaderu a jim odpovidajici struktury
		 */
		static std::map<std::string, PROGRAMBINDING> programs;

		/**
		 * Shadery ktere se pri inicializaci budou nacitat
		 */
		static std::vector<std::string> shadersToLoad;

		/**
		 * Nazvy souboru textur a jim odpovidajici GL objekty
		 */
		static std::map<std::string, GLuint> textures;
		
		/**
		 * Aktualni nabindovana texturovaci jednotka - cache pro zvyseni vykonosti
		 */
		static GLenum activeTexture;

		///////////////////////////////////////////////////////////////
		
		/**
		 * Prohleda zdrojovy kod shaderu a dohleda reference na textury,
		 * ktere nacte a vrati odpovidajici struktury materialu
		 * 
		 * FEATURE: idealni by bylo oddelit prohledani shaderu do 'annotationParseru'
		 *			a semantiku anotaci resit az nasledne
		 */
		static std::vector<TEXTUREBINDING> loadTextures(GLuint program, std::string source);


		/**
		 * Nacte a pripravi program default.vert/frag, ktery slouzi jako fallback pro neexistujici materialy
		 */
		static void loadDefaultProgram();




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
