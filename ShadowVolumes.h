#ifndef SHADOWVOLUMES_H
#define SHADOWVOLUMES_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
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

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include <GL/glu.h>

#include "Debug.h"
#include "Mesh.h"
#include "BaseModel.h"
#include "ShaderManager.h"


/**
 * Trida pro vypocet a vykresleni stinovych teles a stencil stinu
 * inspirace:
 *	http://nehe.gamedev.net/tutorial/shadows/16010/
 *	http://www.angelfire.com/games5/duktroa/RealTimeShadowTutorial.htm
 */
class ShadowVolumes {
	public:
		ShadowVolumes(void);
		~ShadowVolumes(void);

		/**
		 * Pridani modelu, pro ktery se bude stencil volume pocitat
		 */
		inline void addModel(BaseModel* model, glm::mat4 matrix)
		{
			models.push_back(std::pair<BaseModel*, glm::mat4>(model, matrix));
		}

		/**
		 * Pridani zdroje svetla
		 */
		inline void addLight(glm::vec3 position)
		{
			lights.push_back(position);
		}


		/**
		 * Vypocte stinova telesa a a pripravi data ke kresleni;
		 * Po volani teto metody se jiz predane modely nepouzivaji a je mozne je uvolnit
		 */
		void generate();

		/**
		 * Vykresli stinova telesa tvorena svetlem lightI z dane pozice kamery
		 */
		void draw(unsigned int lightI, glm::mat4 mView, glm::mat4 mProjection);

	protected:
		/**
		 * Vypocita sousednosti facu kazde meshe a naplni mapu facesNeighbours,
		 * a soucasne pocita viditelnosti jednotlivych facu ze svetel a uklada do meshFacesVisibilities
		 */
		void computeNeighboursAndVisibilities();

		/** 
		 * Indexy sousednich facu v ramci meshe; -1 znamena ze soused neni 
		 * A je soused hrany x,y; B je soused hrany y,z; C je soused hrany z,x
		 */
		struct Neighbours {
			Neighbours() { a = b = c = -1; }
			int a, b, c;
		};

		/**
		 * Pole meshi sesbiranych ze vsech modelu pro ktere pocitame stiny,
		 * prevedenych do souradneho systemu sveta (vynasobeno modelovou matici)
		 * O alokaci se stara ShadowVolumes
		 */
		std::vector<Mesh*> meshes;

		/**
		 * Pole priznaku pro kazde svetlo, popisujici zda je mesh na odpovidajicim
		 * indexu ze svetla viditelna nebo ne
		 *
		 * meshIndex => [
		 *		faceIndex => [
		 *			lightIndex => true,
		 *			lightIndex => false,
		 *			...
		 *		],
		 *		...
		 * ],
		 * ...
		 */
		std::vector<std::vector<std::vector<bool>>> meshFacesVisibilities;

		/** 
		 * Pole sousednosti kazde meshe - pole obsahuje na kazdem indexu
		 * (ktery odpovida indexu face v meshi) tri hodnoty - indexy sousednich
		 * facu v ramci meshe
		 */
		std::map<Mesh*, std::vector<Neighbours>> facesNeighbours;

		/** Pole modelu a jejich matic pro ktere se budou telesa pocitat */
		std::vector<std::pair<BaseModel*, glm::mat4>> models;

		/** Pole pozic svetel */
		std::vector<glm::vec3> lights;

		/** pole GL bufferu sten stinovych teles pro kazde svetlo */
		std::vector<GLuint> VBOs;
		std::vector<GLuint> EBOs;

		/** pole GL bufferu vik stinovych teles pro kazde svetlo */
		std::vector<GLuint> capVBOs;
		std::vector<GLuint> capEBOs;

		/** pocet indexu facu sten a vik stinovych teles - at vime kolik kreslit */
		unsigned int shadowVolumeIndices;
		unsigned int shadowVolumeCapsIndices;
};


#endif