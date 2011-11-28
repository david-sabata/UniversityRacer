#include "Game.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////////


Game::Game(): mouseCaptured(false), drawWireframe(false), superQueue(NULL)
{

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
	BaseModel* chairs = container->load3DS("models/chairs.3ds");
	BaseModel* e112 = container->load3DS("models/e112.3ds");
    BaseModel* car =  container->load3DS("models/car.3ds");

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
		e112QueueItem = container->queueDraw(e112, modelmat);
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

    {
		container->addModel("car", car);
		carQueueItem = container->queueDraw(car);
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

    cout << "- initializing physics" << endl;

    physics = new Physics();
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, 3, 1));
    physics->AddRigidBody(5., transform, new btBoxShape(btVector3(0.75,0.75,0.75)))->setAngularVelocity(btVector3(1,1,1));
    physics->AddStaticModel(superQueue->at(e112QueueItem).model);

}
 



void Game::onWindowRedraw(const GameTime & gameTime) 
{	
	BaseApp::onWindowRedraw(gameTime);

	handleActiveKeys(gameTime);

    physics->StepSimulation(gameTime.Elapsed() * 0.001f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);    
    glDepthFunc(GL_LESS);
	
	// kazdy snimek upravuju modelovou matici
	// zidli musime ziskavat vzdy znovu, ptz mohlo dojit k realokaci kreslici fronty a ukazatele by nemusely platit
	//ModelContainer::DRAWINGQUEUEITEM &chair = superQueue->at(superChair);
	//chair.matrix = glm::rotate(chair.matrix, 1.0f, glm::vec3(0, 1, 0));
    superQueue->at(superChair).matrix = glm::rotate(superQueue->at(superChair).matrix, gameTime.Elapsed() * 0.1f, glm::vec3(0, 1, 0));

	// vykreslit scenu
	scene->draw();

    // vykreslit fyziku
    physics->DebugDrawWorld();

    SDL_GL_SwapBuffers(); 
}




void Game::handleActiveKeys(const GameTime & gameTime)
{
	bool wDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_w) != activeKeys.end() );
	bool sDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_s) != activeKeys.end() );
	bool aDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_a) != activeKeys.end() );
	bool dDown = ( find(activeKeys.begin(), activeKeys.end(), SDLK_d) != activeKeys.end() );
	
	// chceme aby byla rychlost pohybu nezavisla na fps
	//float f_fps = float(1 / getFPS());
	//float f_step = float(WALK_SPEED / f_fps);
	//float f_step = float(WALK_SPEED / getFPS());
    float f_step = static_cast<float>(gameTime.Elapsed()) * WALK_SPEED;

	// vysledkem jsou slozky vektoru ve smerech X ("strafe", ne otaceni) a Z
	float x = -( (-1.0f * aDown) + (1.0f * dDown) ) * f_step;	
	float z = ( (-1.0f * sDown) + (1.0f * wDown) ) * f_step;		

	camera.Move(x, 0.0f, z);

     if ( find(activeKeys.begin(), activeKeys.end(), SDLK_UP) != activeKeys.end() )
        physics->GetCar()->Forward();
    if ( find(activeKeys.begin(), activeKeys.end(), SDLK_DOWN) != activeKeys.end() )
        physics->GetCar()->Backward();
    if ( find(activeKeys.begin(), activeKeys.end(), SDLK_b) != activeKeys.end() ) 
        physics->GetCar()->HandBrake();
    if ( find(activeKeys.begin(), activeKeys.end(), SDLK_LEFT) != activeKeys.end() )
        physics->GetCar()->TurnLeft();
    if ( find(activeKeys.begin(), activeKeys.end(), SDLK_RIGHT) != activeKeys.end() )
        physics->GetCar()->TurnRight();		
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

    if (key == SDLK_RETURN) {
        physics->GetCar()->Reset();
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

