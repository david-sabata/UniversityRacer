
#include "main.h"

using namespace std;


// Pointer to active application instance, which will recieve and handle SDL events
BaseApp* application = NULL;

SDL_Surface * init(unsigned width, unsigned height, unsigned color, unsigned depth, unsigned stencil)
{
    // Set OpenGL attributes
    if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, color) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) throw SDL_Exception();

    // Create window
    SDL_Surface * screen = SDL_SetVideoMode(width, height, color, SDL_OPENGL | SDL_RESIZABLE);
    if(screen == NULL) throw SDL_Exception();

	SDL_WM_SetCaption("University Racer", "University Racer");

#ifndef USE_GLEE
	// Inicializace glew	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
	// Create the Game and set as current application
	application = new Game;

    // Call init code
    application->onInit();
    application->onWindowResized(width, height);

    return screen;
}




// Simple main loop
void mainLoop()
{
    // Window is not minimized
    bool active = true;

    for(;;) // Infinite loop
    {
        SDL_Event event;

        // Wait for event
        if(SDL_WaitEvent(&event) == 0) throw SDL_Exception();

        // Screen needs redraw
        bool redraw = false;
		
		// aktualizovat FPS
		updateFPS(SDL_GetTicks());
		ostringstream text;
		text << "University Racer - " << setiosflags(ios::fixed) << setprecision(0) << getFPS() << " fps";
		SDL_WM_SetCaption(text.str().c_str(), text.str().c_str());

        // Handle all waiting events
        do
        {
            // Call proper event handlers
            switch(event.type)
            {
                // Stop redraw when minimized
				case SDL_ACTIVEEVENT :
                    if(event.active.state == SDL_APPACTIVE)
                        active = (event.active.gain != 0);
                    break;

				// Set redraw flag
                case SDL_VIDEOEXPOSE :
                    redraw = true;
                    break;
					
				// End main loop
				case SDL_QUIT :
                    return;

				// Pass UI events to the app
                case SDL_KEYDOWN :                    
                case SDL_KEYUP :                    
                case SDL_MOUSEMOTION :                    
                case SDL_MOUSEBUTTONDOWN :                    
                case SDL_MOUSEBUTTONUP :                    
                case SDL_VIDEORESIZE :
					application->handleEvent(event);
					break;

				// Do nothing
                default :
                    break;
            }
        } while(SDL_PollEvent(&event) == 1);

        // Optionally redraw window
        if (active && redraw) application->onWindowRedraw();
    }
}



// Animation main loop
// period - maximum time between redraws in ms
void mainLoop(unsigned int period)
{
    // This main loop requires timer support
    if(SDL_InitSubSystem(SDL_INIT_TIMER) < 0) throw SDL_Exception();

    // Create redraw timer
    class RedrawTimer
    {
        private :
            SDL_TimerID id;
            static Uint32 callback(Uint32 interval, void *)
            {
                SDL_Event event;
				event.type = SDL_VIDEOEXPOSE;
				if(SDL_PushEvent(&event) < 0) throw SDL_Exception();

                return interval;
            }
        public :
            RedrawTimer(unsigned interval)
                : id(SDL_AddTimer(interval, callback, NULL))
            {
                if(id == NULL) throw SDL_Exception();
            }
            ~RedrawTimer()
            {
                if(id != NULL) SDL_RemoveTimer(id);
            }
    } redrawTimer(period);

    // Start simple main loop
    mainLoop();
}





int main(int /*argc*/, char ** /*argv*/) 
{
	try {
        // Init SDL - only video subsystem will be used
        if(SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

        // Shutdown SDL when program ends
        atexit(SDL_Quit);

		// initialize application
        init(800, 600, 24, 24, 8);

		// start the main loop
		mainLoop(16); // 16ms = cca 60fps

		// cleanup
		delete application;

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;        
		system("pause");
		return EXIT_FAILURE;
    }

	//system("pause");
    return EXIT_SUCCESS;
}

/**
 * Pomocna funkce pro GLEE/GLEW kompatibilitu
 */
const char * getGlErrorString(GLenum error)
{
#define ERROR(e) case e : return #e
    switch(error)
    {
        ERROR(GL_NO_ERROR);
        ERROR(GL_INVALID_ENUM);
        ERROR(GL_INVALID_VALUE);
        ERROR(GL_INVALID_OPERATION);
        ERROR(GL_INVALID_FRAMEBUFFER_OPERATION);
        ERROR(GL_OUT_OF_MEMORY);
    default : 
        return "UNKNOWN_GL_ERROR";
    }
#undef ERROR
}
