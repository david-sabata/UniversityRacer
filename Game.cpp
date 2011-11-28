#include "Game.h"
#include "Light.h"

using namespace std;



#define WALK_SPEED 50


// pomocna promenna pro moznost kreslit wireframe (TAB)
bool drawWireframe = false;



ModelContainer * pohybSvetla;
// pomocna promenna pro zapamatovani si indexu polozky kreslici fronty
unsigned int superChair;
// ukazatel na frontu ve ktere se zidle nachazi
vector<ModelContainer::DRAWINGQUEUEITEM>* superQueue = NULL;


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

	cout << "The Game is loading:" << endl;

	cout << "- loading models" << endl;

	// nacist modely	
	ModelContainer* container = new ModelContainer;	

	BaseModel* room = container->load3DS("models/test.3ds");
	container->addModel("room", room);
	
	glm::mat4 mat = glm::scale(glm::vec3(0.1));
	container->queueDraw(room, mat);
	pohybSvetla = container;
#if 0
	BaseModel* chairs = container->load3DS("models/chairs.3ds");
	BaseModel* e112 = container->load3DS("models/e112.3ds");

	/*
	CachedModel* e112 = CachedModel::load("models/e112.3ds~");
	if (e112 == NULL) {
		BaseModel* tmp = container->load3DS("models/e112.3ds");
		e112 = new CachedModel(tmp, "models/e112.3ds~");
	}
	*/
	
	cout << "- setting up drawing queue" << endl;


	
	// vykresli E112 zmensenou na 20%
	if (1) {
		container->addModel("e112", e112);
		glm::mat4 modelmat = glm::scale(glm::vec3(0.2));
		container->queueDraw(e112, modelmat);
	}

	// vykresli zidle
	{
		glm::mat4 scale = glm::scale(glm::vec3(0.2));
		glm::mat4 row0 = glm::translate(scale, glm::vec3(0, 19, -70));

		container->addModel("chairs", chairs);
		
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 col = glm::translate(row0, glm::vec3(110 * i, 0, 0));
			superChair = container->queueDraw(chairs, col); // jen testovaci; ulozi se index na posledni pridanou zidli
		}		
	}
#endif

	// pro kazde svetlo v kontejneru pridat kouli, ktera ho znazornuje
	{
		BaseModel* sphere = container->load3DS("models/sphere.3ds");
		container->addModel("lightsphere", sphere);

		vector<Light> lights = container->getLights();
		for (vector<Light>::iterator it = lights.begin(); it != lights.end(); it++)
		{
			glm::vec4 pos = (*it).Position();
			glm::mat4 mat = glm::scale(glm::translate(pos.x, pos.y, pos.z), glm::vec3(0.01));
			//glm::mat4 mat = glm::translate(glm::scale(glm::vec3(0.01)), pos.x, pos.y, pos.z);
		
			container->queueDraw(sphere, mat);
		}
	}


	// zapamatovat si frontu
	superQueue = &container->getDrawingQueue();

	cout << "- constructing scene" << endl;

	// vyrobit scenu
	scene = new Scene(*this);
	scene->addModelContainer(container);
	scene->init();
	
	cout << "- done!" << endl;

	// nacist vsechny materialy
	ShaderManager::loadPrograms();

	glEnable(GL_CULL_FACE);
}
 



void Game::onWindowRedraw() 
{	
	BaseApp::onWindowRedraw();

	handleActiveKeys();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);    
    glDepthFunc(GL_LESS);
	
	// kazdy snimek upravuju modelovou matici
	// zidli musime ziskavat vzdy znovu, ptz mohlo dojit k realokaci kreslici fronty a ukazatele by nemusely platit
	//ModelContainer::DRAWINGQUEUEITEM &chair = superQueue->at(superChair);
	//chair.matrix = glm::rotate(chair.matrix, 1.0f, glm::vec3(0, 1, 0));

	// vykreslit scenu
	scene->draw();

    SDL_GL_SwapBuffers(); 
}




void Game::handleActiveKeys()
{
	bool wDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_w) != activeKeys.end() );
	bool sDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_s) != activeKeys.end() );
	bool aDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_a) != activeKeys.end() );
	bool dDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_d) != activeKeys.end() );
	
	//TMP - pohyb svetla !!!
	bool iDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_i) != activeKeys.end() );
	bool kDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_k) != activeKeys.end() );
	bool jDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_j) != activeKeys.end() );
	bool lDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_l) != activeKeys.end() );
	
		//TMP!!!!!!
	Light l = (*pohybSvetla->getLights().begin());
	if(iDown) pohybSvetla->getLights().at(0).Position().z+=40.0f;
	if(kDown) pohybSvetla->getLights().at(0).Position().z-=40.0f;
	if(jDown) pohybSvetla->getLights().at(0).Position().x-=40.0f;
	if(lDown) pohybSvetla->getLights().at(0).Position().x+=40.0f;
	
	cout << "Pozice svetla Z: " << l.position.z << "X: " << l.position.x << endl;
	
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

	// TAB prepina mezi vyplnenym kreslenim a wireframe
	if (key == SDLK_TAB) {
		drawWireframe = !drawWireframe;

		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

