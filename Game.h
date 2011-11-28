#ifndef GAME_H
#define GAME_H

#include <algorithm>

#include "BaseApp.h"
#include "Scene.h"
#include "CachedModel.h"
#include "Physics.h"

static const float WALK_SPEED = 0.1f;

class Game : public BaseApp
{

	public:
		Game();
		~Game();
		void onInit();
		void onWindowRedraw(const GameTime & gameTime);
		
		void handleActiveKeys(const GameTime & gameTime);
		
		void onKeyDown(SDLKey key, Uint16 mod);
		void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons);

	protected:
		Scene* scene;
        Physics *physics;

		bool mouseCaptured;

        // pomocna promenna pro moznost kreslit wireframe (TAB)
        bool drawWireframe;

        // pomocna promenna pro zapamatovani si indexu polozky kreslici fronty
        unsigned int superChair, carQueueItem, e112QueueItem;
        // ukazatel na frontu ve ktere se zidle nachazi
        std::vector<ModelContainer::DRAWINGQUEUEITEM>* superQueue;

};

#endif