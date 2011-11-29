#ifndef GAME_H
#define GAME_H

#include <algorithm>

#include "BaseApp.h"
#include "Scene.h"
#include "CachedModel.h"

class Game : public BaseApp
{

	public:
		Game();
		~Game();
		void onInit();
		void onWindowRedraw(const GameTime & gameTime);
		
		// vykresli usecku mezi dvema body (souradnice jsou v prostoru sceny)
		void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color);

		void handleActiveKeys(const GameTime & gameTime);		
		void onKeyDown(SDLKey key, Uint16 mod);
		void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons);

	protected:
		Scene* scene;

		bool mouseCaptured;

        // ukazatel na frontu kreslenych objektu
        std::vector<ModelContainer::DRAWINGQUEUEITEM>* drawingQueue;
        // pomocna promenna pro moznost kreslit wireframe (TAB)
        bool drawWireframe;

};

#endif