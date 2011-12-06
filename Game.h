#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>

#include "Debug.h"
#include "BaseApp.h"
#include "Scene.h"
#include "Gui.h"
#include "Debug.h"
#include "Physics.h"


class Game : public BaseApp
{

	public:
		Game();
		~Game();
		void onInit();
		void onWindowRedraw(const GameTime & gameTime);
		void onWindowResized(int w, int h);
		
		// vykresli usecky
        void drawLines(std::vector<PhysicsDebugDraw::LINE> & lines);

		void handleActiveKeys(const GameTime & gameTime);		
		void onKeyDown(SDLKey key, Uint16 mod);
		void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons);

        // vraci statistiku - pocet vrcholu, facu,...
        std::string statsString();

	protected:
		Scene* scene;
		Gui* gui;
        ModelContainer* container;
        Physics *physics;

		// handle retezce ukazujici cas
		unsigned int guiTime;

		bool mouseCaptured;

        // ukazatel na frontu kreslenych objektu
        //std::vector<ModelContainer::DRAWINGQUEUEITEM>* drawingQueue;

        // indexy polozek v kreslici fronte
        unsigned int carQueueItem, e112QueueItem;
        unsigned int wheelQueueItem[4]; // z pohledu ridice: 0 - predni leve, 3 - zadni prave

        // pomocna promenna pro moznost kreslit wireframe (TAB)
        bool drawWireframe;

        // pomocna promenna pro moznost zapnuti kamery ktera nasleduje auto (F)
        bool followCamera;

};

#endif