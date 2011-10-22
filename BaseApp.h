#ifndef BASEAPP_H
#define BASEAPP_H

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include "Exceptions.h"
#include "Utils.h"
#include "ModelContainer.h"
#include "Scene.h"
#include "ShaderManager.h"



class BaseApp {
	
	public:
		BaseApp();

		virtual void handleEvent(SDL_Event);
		
		virtual void onInit();
		virtual void onWindowRedraw();		

		virtual void onWindowResized(int w, int h);
		virtual void onKeyDown(SDLKey key, Uint16 /*mod*/);
		virtual void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/);
		virtual void onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons);
		virtual void onMouseDown(Uint8 button, unsigned /*x*/, unsigned /*y*/);
		virtual void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/);


	protected:
		// viewport dimensions
		int windowWidth, windowHeight;

		// mouse
		float mouseRX, mouseRY, mouseWheel;

		// Z-axis // TODO: use Camera
		float pz;

		////////////////////////////////////////////////////////////////////////////////
		
		// Send quit event
		inline void quit()
		{
			SDL_Event event;
			event.type = SDL_QUIT;
			if(SDL_PushEvent(&event) < 0) throw SDL_Exception();
		}

		// Send redraw event
		inline void redraw()
		{
			SDL_Event event;
			event.type = SDL_VIDEOEXPOSE;
			if(SDL_PushEvent(&event) < 0) throw SDL_Exception();
		}
};



#endif