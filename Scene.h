#ifndef SCENE_H
#define SCENE_H

#include <iostream>

#include <GL/glew.h>

#include "ModelContainer.h"
#include "ShaderManager.h"

class Scene
{

	public:
		~Scene();

		/**
		 * Prida do sceny novy kontejner modelu
		 */
		void addModelContainer(ModelContainer* container);

		/**
		 * Inicializace sceny;
		 * Po inicializaci se jiz neprojevi nove pridana data!
		 */
		void init();

		/**
		 * Vykresleni cele sceny
		 */
		void draw(glm::mat4 mvp);


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

		////////////////////////////////////////////////////////////////

		/**
		 * Sestavi z modelu prislusne VBO a EBO
		 */
		void buildBufferObjects();

};

#endif