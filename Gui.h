#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include <utility>

#include "Debug.h"
#include "ShaderManager.h"


/**
 * Trida zajistujici vykresleni GUI
 * 
 * Kresli v rovnobeznem promitani ctverce potazene texturami/barvami
 */
class Gui {
	public:
		
		Gui(int screenWidth, int screenHeight);

		/**
		 * Enumeratory pro pozice na kreslici plose
		 */
		enum {
			TOP = 0,
			LEFT = 0,
			CENTER = 50,
			BOTTOM = 100,
			RIGHT = 100
		};

		/**
		 * Pozice prvku na kreslici plose; hodnoty vyjadruji procenta
		 * a [0,0] je v levem hornim rohu.
		 * Je mozne pouzit cisla, pripadne hodnoty ENUMu
		 */
		typedef struct {
			int top;			
			int left;
		} POSITION;


		/**
		 * Souradnice ctyr bodu na texture
		 */
		typedef struct {
			glm::vec2 topleft;
			glm::vec2 topright;
			glm::vec2 btmright;
			glm::vec2 btmleft;
		} LTRTEXCOORDS;


		/**
		 * Vlozi retezec na zadanou pozici a vrati handle retezce,
		 * pomoci ktereho jej lze upravovat ci odebrat
		 */
		unsigned int addString(std::string str, POSITION pos)
		{
			std::pair<std::string, POSITION> p(str, pos);
			strings.push_back(p);
			updateData();
			return strings.size() - 1;
		}

		/**
		 * Odebere retezec
		 */
		inline void removeString(unsigned int handle)
		{
			strings.erase(strings.begin() + handle);
			updateData();
		}


		/**
		 * Nastavi retezci novy text
		 */
		inline void updateString(unsigned int handle, std::string text)
		{
			strings.at(handle).first = text;
			updateData();
		}
		
		
		/**
		 * Nastaveni novych rozmeru kreslici plochy v pripade ze se zmenily
		 */
		inline void updateScreenDimensions(int screenWidth, int screenHeight) 
		{
			this->screenWidth = screenWidth;
			this->screenHeight = screenHeight;
			updateData();
		}


		/**
		 * Vykresleni vsech prvku rozhrani
		 */
		void draw();



	protected:
		/** 
		 * Obnovi data ze kterych se kresli 
		 */
		void updateData();

		/** 
		 * Vypocte souradnice znaku v texture (4 rohy 0..1)
		 */
		LTRTEXCOORDS getCharTexCoords(char c);


		ShaderManager::MATERIALPARAMS materialParams;


		std::vector<GLfloat> vertices;	// 2 floaty == bod
		std::vector<GLfloat> texcoords;	// 2 floaty == souradnice
		std::vector<GLuint> indices;		// 1 int == index, 4 inty == quad

		/** Rozmery dostupne kreslici plochy */
		int screenWidth;
		int screenHeight;

		/** Pole retezcu na kreslici plose; handle odpovidaji indexum */
		std::vector< std::pair<std::string, POSITION> > strings;
};


#endif