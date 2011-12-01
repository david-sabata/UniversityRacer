#include "BaseApp.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


BaseApp::BaseApp() 
{
	/*
	mouseRX = 0.0f;
	mouseRY = 0.0f;
	mouseWheel = 1.0f;

	pz = -100.0f;	
	*/

	windowWidth = 0;
	windowHeight = 0;
}

BaseApp::~BaseApp()
{
}


Camera* const BaseApp::getCamera()
{
	return &camera;
}



// Call proper event handlers
void BaseApp::handleEvent(SDL_Event event)
{	
	switch(event.type)
	{		
		case SDL_KEYDOWN :
			onKeyDown(event.key.keysym.sym, event.key.keysym.mod);			
			break;
		case SDL_KEYUP :
			onKeyUp(event.key.keysym.sym, event.key.keysym.mod);
			break;
		case SDL_MOUSEMOTION :
			onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, event.motion.state);
			break;
		case SDL_MOUSEBUTTONDOWN :
			onMouseDown(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_MOUSEBUTTONUP :
			onMouseUp(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_VIDEORESIZE:
			onWindowResized(event.resize.w, event.resize.h);
			break;
		/*case SDL_VIDEOEXPOSE:
			onWindowRedraw();
			break;*/
		default :
			break;
	}
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Default event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BaseApp::onWindowResized(int w, int h)
{
	glViewport(0, 0, w, h);
	windowWidth = w;
	windowHeight = h;
}

void BaseApp::onKeyDown(SDLKey key, Uint16 /*mod*/)
{
	activeKeys.remove(key); // chceme mit kazdou klavesu jen jednou
	activeKeys.push_back(key);

	switch(key) {
		case SDLK_ESCAPE : quit(); return;
		default : return;
	}
}


void BaseApp::onKeyUp(SDLKey key, Uint16 /*mod*/)
{
	activeKeys.remove(key);
}

void BaseApp::onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons)
{
	/*
	if(buttons & SDL_BUTTON_LMASK)
	{
		mouseRX += xrel;
		mouseRY += yrel;
		redraw();
	}
	if(buttons & SDL_BUTTON_RMASK)
	{
		pz += yrel;
		redraw();
	}
	*/	
}

void BaseApp::onMouseDown(Uint8 button, unsigned /*x*/, unsigned /*y*/)
{
	/*
	switch(button) {
		case SDL_BUTTON_WHEELUP : mouseWheel += 1; break;
		case SDL_BUTTON_WHEELDOWN : mouseWheel -= 1; break;
		default : return;
	};
	redraw();
	*/
}

void BaseApp::onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}


void BaseApp::onWindowRedraw(const GameTime & gameTime)
{	
}

void BaseApp::onInit()
{
}


