#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <iostream>
#include <map>

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Debug.h"
#include "ModelContainer.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "BaseApp.h"


// forward-declaration; Scene a BaseApp se potrebuji vzajemne znat
class BaseApp;


class Scene
{

	public:
		Scene(BaseApp& parentApp);
		~Scene();

		/**
		 * Prida do sceny novy kontejner modelu
		 */
		void addModelContainer(ModelContainer* container);

		/**
		 * Vraci referenci na kontejnery jiz pridane do sceny
		 */
		std::vector<ModelContainer*> &getModelContainers();

		/**
		 * Inicializace sceny;
		 * Po inicializaci se jiz neprojevi nove pridana data!
		 */
		void init();

		/**
		 * Vykresleni cele sceny
		 * Moznost zvolit, ktere slozky se budou kreslit a predat pole
		 * priznaku rozsvicenych svetel
		 */
		void draw(bool drawAmbient = true, bool drawLighting = true, std::vector<bool> enabledLights = std::vector<bool>());


	protected:
		/**
		 * Kontejnery modelu ktere jsou ve scene
		 */
		std::vector<ModelContainer*> containers;

		/**
		 * GL VBO - kazdy odpovida jednomu kontejneru
		 * na odpovidajicim idnexu
		 */
		std::vector<GLuint> VBOs;

		/**
		 * GL EBO - kazdy odpovida jednomu kontejneru
		 * na odpovidajicim indexu
		 */
		std::vector<GLuint> EBOs;

		/**
		 * Reference na objekt aplikace, ktera scenu stvorila
		 */
		BaseApp& application;
		
		////////////////////////////////////////////////////////////////

		/**
		 * Sestavi z modelu prislusne VBO a EBO
		 */
		void buildBufferObjects();

		void setProgramUniforms(ShaderManager::PROGRAMBINDING binding);

};

#endif
