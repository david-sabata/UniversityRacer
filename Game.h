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
		void onWindowRedraw();
		
		void handleActiveKeys();
		
		void onKeyDown(SDLKey key, Uint16 mod);
		void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons);

	protected:
		Scene* scene;

		bool mouseCaptured;

};

#endif