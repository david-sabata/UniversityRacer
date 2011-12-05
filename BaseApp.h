#ifndef BASEAPP_H
#define BASEAPP_H

#include <list>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include "Debug.h"
#include "Exceptions.h"
#include "Utils.h"
#include "ModelContainer.h"
#include "Scene.h"
#include "ShaderManager.h"
#include "Camera.h"



class BaseApp {
	
	public:
		BaseApp();
		virtual ~BaseApp();

		Camera* const getCamera();		

		virtual void handleEvent(SDL_Event);
		
		virtual void onInit();
		virtual void onWindowRedraw(const GameTime & gameTime);		

		virtual void onWindowResized(int w, int h);
		virtual void onKeyDown(SDLKey key, Uint16 /*mod*/);
		virtual void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/);
		virtual void onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons);
		virtual void onMouseDown(Uint8 button, unsigned /*x*/, unsigned /*y*/);
		virtual void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/);

		// Vraci pomer stran okna aplikace
		inline double getWindowAspectRatio()
		{
			return static_cast<double>(windowWidth) / windowHeight;
		}


	protected:
		// rozmery viewportu
		int windowWidth, windowHeight;

		// camera
		Camera camera;

		// seznam aktualne stisknutych klaves
		std::list<SDLKey> activeKeys;
		
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
