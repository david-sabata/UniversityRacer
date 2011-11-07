#include "Game.h"

using namespace std;



#define WALK_SPEED 50


////////////////////////////////////////////////////////////////////////////////


Game::Game()
{
	mouseCaptured = false;
}

Game::~Game()
{
	delete scene;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Game::onInit()
{
	BaseApp::onInit();

	cout << "The Game is loading..." << endl;

	// nacist modely 
	// (muze byt pomalejsi - automaticky generuje per-vertex normaly)
	ModelContainer* container = new ModelContainer;
	BaseModel* e112 = container->load3DS("models/e112.3ds");
	
	// vykresli E112 zmensenou na 20%
	container->addModel("e112", e112);
	glm::mat4 modelmat = glm::scale(glm::vec3(0.2));	
	container->queueDraw(e112, modelmat);


	// vyrobit scenu
	scene = new Scene(*this);
	scene->addModelContainer(container);
	scene->init();

	cout << "... done!" << endl;

	// nacist vsechny materialy
	ShaderManager::loadMaterial("default");	
}
 



void Game::onWindowRedraw() 
{	
	BaseApp::onWindowRedraw();

	handleActiveKeys();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);    
    glDepthFunc(GL_LESS);
	
	/*
	float aspect = (float)windowWidth/(float)windowHeight;

	projection = glm::perspective(10.0f*mouseWheel, aspect, 1.0f, 1000.0f);
    
    glm::mat4 mvp = glm::rotate(
            glm::rotate(
                glm::translate(
                    projection,
                    glm::vec3(0, 0, pz)
                    ),
                mouseRY, glm::vec3(1, 0, 0)
                ),
            mouseRX, glm::vec3(0, 1, 0)
            );

    //glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
	*/

	scene->draw();

    SDL_GL_SwapBuffers(); 
}




void Game::handleActiveKeys()
{
	bool wDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_w) != activeKeys.end() );
	bool sDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_s) != activeKeys.end() );
	bool aDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_a) != activeKeys.end() );
	bool dDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_d) != activeKeys.end() );
	
	// chceme aby byla rychlost pohybu nezavisla na fps
	//float f_fps = float(1 / getFPS());
	//float f_step = float(WALK_SPEED / f_fps);
	float f_step = float(WALK_SPEED / getFPS());

	// vysledkem jsou slozky vektoru ve smerech X ("strafe", ne otaceni) a Z
	float x = -( (-1.0f * aDown) + (1.0f * dDown) ) * f_step;	
	float z = ( (-1.0f * sDown) + (1.0f * wDown) ) * f_step;		

	camera.Move(x, 0.0f, z);
	
}



void Game::onKeyDown(SDLKey key, Uint16 mod)
{
	BaseApp::onKeyDown(key, mod);

	// mezernik zamyka mys pro ovladani kamery
	if (key == SDLK_SPACE) {
		mouseCaptured = !mouseCaptured;
		SDL_ShowCursor(!mouseCaptured);

		if (mouseCaptured)
			SDL_WM_GrabInput(SDL_GRAB_ON);
		else
			SDL_WM_GrabInput(SDL_GRAB_OFF);
	}
}


void Game::onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons)
{
	BaseApp::onMouseMove(x, y, xrel, yrel, buttons);


	if(mouseCaptured) {					
		float vertAngle = -yrel * 0.001f;
		float horizAngle = -xrel * 0.001f;

		camera.Aim(vertAngle, horizAngle);

		if (0) {
			cout << "xrel: " << xrel << "\tyrel: " << yrel << endl;
			camera.DebugDump();
		}
	}
}

