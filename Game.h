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
#include "ShadowVolumes.h"


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
		ShadowVolumes* shadowVolumes;

		// handle retezce ukazujici cas
		unsigned int guiTime, guiCheckpoint;

		bool mouseCaptured;

        // indexy polozek v kreslici fronte
        unsigned int carQueueItem, wheelQueueItem[PhysicsCar::WHEEL_COUNT];

        // pomocna promenna pro moznost kreslit wireframe (TAB)
        bool drawWireframe;

        // pomocna promenna pro moznost zapnuti kamery ktera nasleduje auto (F)
        bool followCamera;

		// pole priznaku rozsvicenych a zhasnutych svetel (moznost prepinani klavesami 1-9)
		std::vector<bool> enabledLights;

		// kreslit stiny? (F6)
		bool drawShadows;

		// zobrazujeme napovedu? (F1)
		bool showHelp;
};

#endif