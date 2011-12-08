#include "Game.h"
#include "Light.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


#define WALK_SPEED 0.01f


////////////////////////////////////////////////////////////////////////////////


Game::Game(): mouseCaptured(false), /*drawingQueue(NULL),*/ drawWireframe(false), followCamera(false)
{
	gui = new Gui(windowWidth, windowHeight);
	scene = new Scene(*this);
}

Game::~Game()
{
	delete gui;
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
	container = new ModelContainer;	


	BaseModel* chairs = container->load3DS("models/chairs.3ds");
	BaseModel* e112 = container->load3DS("models/e112.3ds");
	BaseModel* middesk = container->load3DS("models/desk-mid.3ds");
  	BaseModel* sidedesk = container->load3DS("models/desk-side.3ds");
    BaseModel* car =  container->load3DS("models/car.3ds");
    BaseModel* wheel =  container->load3DS("models/wheel.3ds");

    cout << "- initializing physics" << endl;

    physics = new Physics();
    physics->AddCar(PhysicsUtils::btTransFrom(btVector3(37.19f, 10.71f, -21.7f), btQuaternion(btVector3(0, 1, 0), -3.14159f/2.f))); // 0,2,5
    //physics->AddCar(PhysicsUtils::btTransFrom(btVector3(8.69, 2.63, -5.4), btQuaternion(btVector3(0, 1, 0), -M_PI/2.f))); // 0,2,5    
    //physics->AddRigidBody(5., PhysicsUtils::btTransFrom(btVector3(0, 3, 1)), new btBoxShape(btVector3(0.75,0.75,0.75)))->setAngularVelocity(btVector3(1,1,1)); // TODO konstruktor se neprelozi kvuli Debug.h    
    		
	cout << "- setting up drawing queue" << endl;

    float staticScale = 0.05f;
		
	// vykresli E112 zmensenou na 20%
	if (1) {
		container->addModel("e112", e112);
		glm::mat4 modelmat = glm::scale(glm::vec3(staticScale));
		e112QueueItem = container->queueDraw(e112, modelmat);

        physics->AddStaticModel(Physics::CreateStaticCollisionShapes(e112, staticScale), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false);
	}

    // vykresli zidle
	{
		container->addModel("chairs", chairs);
                
		glm::mat4 scale = glm::scale(glm::vec3(staticScale));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-740, 19, -70)),
			glm::translate(scale, glm::vec3(-740, 39, -170)),
			glm::translate(scale, glm::vec3(-740, 59, -270)),
			glm::translate(scale, glm::vec3(-740, 79, -370)),
			glm::translate(scale, glm::vec3(-740, 99, -470))
		};

        std::vector<btCollisionShape*> chairShapes = Physics::CreateStaticCollisionShapes(chairs, staticScale);
		
		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			int offsetX = 0; // posunuti zidle na radku

			for (unsigned int i = 0; i < 13; i++)
			{				
				if (i == 3 || i == 10)
					offsetX += 100;

				glm::mat4 col = glm::translate(rows[rowI], glm::vec3(offsetX, 0, 0));
				container->queueDraw(chairs, col); // jen testovaci; ulozi se index na posledni pridanou zidli
                offsetX += 105;

                physics->AddStaticModel(chairShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/staticScale))), false);
			}		
		}
	}

	// vykresli prostredni lavice
	{
		container->addModel("middesk", middesk);

		glm::mat4 scale = glm::scale(glm::vec3(staticScale));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-365, 13, -43)),
			glm::translate(scale, glm::vec3(-365, 33, -143)),
			glm::translate(scale, glm::vec3(-365, 53, -243)),
			glm::translate(scale, glm::vec3(-365, 73, -343)),
			glm::translate(scale, glm::vec3(-365, 93, -443))
		};

        std::vector<btCollisionShape*> middeskShapes = Physics::CreateStaticCollisionShapes(middesk, staticScale);

		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			glm::mat4 col = glm::translate(rows[rowI], glm::vec3(0, 0, 0));
			container->queueDraw(middesk, col);

            physics->AddStaticModel(middeskShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/staticScale))), false);
		}
	}

	// vykresli postranni lavice
	{
		container->addModel("sidedesk", sidedesk);

		glm::mat4 scale = glm::scale(glm::vec3(staticScale));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-785,  20, -15)),
			glm::translate(scale, glm::vec3(-785,  40, -115)),
			glm::translate(scale, glm::vec3(-785,  60, -215)),
			glm::translate(scale, glm::vec3(-785,  80, -315)),
			glm::translate(scale, glm::vec3(-785, 100, -415))
		};

        std::vector<btCollisionShape*> sidedeskShapes = Physics::CreateStaticCollisionShapes(sidedesk, staticScale);

		glm::vec3 otherside(1250, 0, 0);

		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			// leva strana (z pohledu z katedry)
			glm::mat4 col = glm::translate(rows[rowI], glm::vec3(0, 0, 0));			
			container->queueDraw(sidedesk, col);

            physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/staticScale))), false);

			// prava strana
			glm::mat4 mat = glm::translate(col, otherside);
			container->queueDraw(sidedesk, mat);

            physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(glm::scale(mat, glm::vec3(1/staticScale))), false);
		}
	}

	// pro kazde svetlo v kontejneru pridat kouli, ktera ho znazornuje
	if (1) {
		BaseModel* sphere = container->load3DS("models/sphere.3ds");
		container->addModel("lightsphere", sphere);

		vector<Light> lights = container->getLights();
		for (vector<Light>::iterator it = lights.begin(); it != lights.end(); it++)
		{
			glm::vec4 pos = (*it).Position();
			glm::mat4 mat = glm::scale(glm::translate(pos.x, pos.y, pos.z), glm::vec3(0.001));
			//glm::mat4 mat = glm::translate(glm::scale(glm::vec3(0.01)), pos.x, pos.y, pos.z);
		
			container->queueDraw(sphere, mat);
		}
	}

    {
		container->addModel("car", car);
		carQueueItem = container->queueDraw(car);
	}

    {
		container->addModel("wheel", wheel);
		for (unsigned int i = 0; i < 4; i++)
        {
            wheelQueueItem[i] = container->queueDraw(wheel);
        }
	}

	cout << "- constructing scene" << endl;

	// vyrobit scenu
	scene->addModelContainer(container);
	scene->init();
	
	cout << "- done!" << endl;
	
	// nacist vsechny materialy	
	ShaderManager::loadPrograms();

	// testovaci gui text	
	Gui::POSITION pos = {Gui::TOP, Gui::LEFT};
	guiTime = gui->addString(".", pos);

	ShaderManager::loadProgram("line");
}
 



void Game::onWindowRedraw(const GameTime & gameTime) 
{	
	BaseApp::onWindowRedraw(gameTime);

	handleActiveKeys(gameTime);

    physics->StepSimulation(gameTime.Elapsed() * 0.001f);
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);    
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

    glDepthFunc(GL_LESS);

    glm::mat4 carMatrix = PhysicsUtils::glmMat4From(physics->GetCar()->GetWorldTransform());
    carMatrix = glm::scale(carMatrix, glm::vec3(CAR_SCALE));
    container->updateDrawingMatrix(carQueueItem, carMatrix);

    if (followCamera)
    {
        btVector3 vel = physics->GetCar()->GetVehicle()->getRigidBody()->getLinearVelocity();        
        camera.Follow(carMatrix, glm::vec3(vel.x(), vel.y(), vel.z()), gameTime);
    }    
 
    for (int i = 0; i < physics->GetCar()->GetVehicle()->getNumWheels(); i++)
    {
        physics->GetCar()->GetVehicle()->updateWheelTransform(i, true); //synchronize the wheels with the (interpolated) chassis worldtransform        
        glm::mat4 wheelMatrix = PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(i).m_worldTransform);
        
        if (i == CarPhysics::WHEEL_FRONTRIGHT || i == CarPhysics::WHEEL_REARRIGHT)
            wheelMatrix = glm::rotate(wheelMatrix, 180.f, 0.f, 1.f, 0.f);

        wheelMatrix = glm::scale(wheelMatrix, glm::vec3(CAR_SCALE));

        container->updateDrawingMatrix(wheelQueueItem[i], wheelMatrix);
    }
	
	// vykreslit scenu
	scene->draw();

    if (drawWireframe)
    {
        // vykreslit fyziku
        physics->DebugDrawWorld();

        /*for (int i = 0; i < 100000; i++)
        {
            physics->GetDebugDrawer()->drawLine(btVector3(0,0,0), btVector3(5,5,5), btVector3(1,0,0));
            physics->GetDebugDrawer()->drawLine(btVector3(6,6,6), btVector3(11,11,11), btVector3(0,1,0));
        }*/

        drawLines(physics->GetDebugDrawer()->GetLines());
    }
    
    // ---------------------------------------
	// Vykresleni ingame gui
	
	ostringstream time;
    time << physics->GetCar()->GetVehicle()->getCurrentSpeedKmHour(); //gameTime.Total();

	gui->updateString(guiTime, time.str());
	gui->draw();


	// ---------------------------------------

    SDL_GL_SwapBuffers(); 
}

void Game::drawLines(vector<PhysicsDebugDraw::LINE> & lines)
{
    // vykresleni car -----------------------
	glDisable(GL_CULL_FACE);

	vector<GLfloat> vertices;
	vector<GLuint> indices;
	unsigned int indexI = 0;

	for (vector<PhysicsDebugDraw::LINE>::iterator it = lines.begin(); it != lines.end(); it++)
	{
		PhysicsDebugDraw::LINE l = (*it);
		vertices.push_back(l.a.x);
		vertices.push_back(l.a.y);
		vertices.push_back(l.a.z);

		vertices.push_back(l.b.x);
		vertices.push_back(l.b.y);
		vertices.push_back(l.b.z);

		indices.push_back(indexI);
		indices.push_back(indexI + 1);
		indexI += 2;
	}
	
	ShaderManager::PROGRAMBINDING activeBinding = ShaderManager::useProgram("line");
	glDisableVertexAttribArray(activeBinding.iEnabledLightsUniform);
	glDisableVertexAttribArray(activeBinding.vLightsUniform);
	glDisableVertexAttribArray(activeBinding.mMVInverseTranspose);
	glDisableVertexAttribArray(activeBinding.normalAttrib);
	glDisableVertexAttribArray(activeBinding.tangentAttrib);
	glDisableVertexAttribArray(activeBinding.texposAttrib);
	glDisableVertexAttribArray(activeBinding.matParams.ambient);
	glDisableVertexAttribArray(activeBinding.matParams.diffuse);
	glDisableVertexAttribArray(activeBinding.matParams.specular);
	glDisableVertexAttribArray(activeBinding.matParams.shininess);

	// vrcholy
	glEnableVertexAttribArray(activeBinding.positionAttrib);
	glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)&(vertices.at(0)));

	// modelova matice
	glm::mat4 modelmat = glm::mat4(1.0);
	glUniformMatrix4fv(activeBinding.mModelUniform, 1, GL_FALSE, glm::value_ptr(modelmat));

	// pohledova matice
	glm::mat4 mView = getCamera()->GetMatrix();
	glUniformMatrix4fv(activeBinding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));

	// projekcni matice
	glm::mat4 mProjection = glm::perspective(45.0f, (float)getWindowAspectRatio(), 0.1f, 1000.0f);
	glUniformMatrix4fv(activeBinding.mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjection));
	
	// nastaveni kamery
	glm::vec3 eye = getCamera()->getEye();
	glm::vec3 sight = getCamera()->getTarget();
	GLuint eyeUniform = glGetUniformLocation(activeBinding.program, "eye");
	GLuint sightUniform = glGetUniformLocation(activeBinding.program, "sight");
	glUniform3f(eyeUniform, eye.x, eye.y, eye.z);
	glUniform3f(sightUniform, sight.x, sight.y, sight.z);

	// kresleni car
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, (void*)&(indices.at(0)));

    lines.clear();
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
    float f_step = gameTime.Elapsed() * WALK_SPEED;

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
        camera.ResetFollow();
    }

    if (key == SDLK_f)
        followCamera = !followCamera;

    if (key == SDLK_c)
        camera.DebugDump();
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


void Game::onWindowResized(int w, int h)
{
	BaseApp::onWindowResized(w, h);
	gui->updateScreenDimensions(w, h);
}


string Game::statsString()
{
    static unsigned int vertCount = 0;
    static unsigned int faceCount = 0;
    
    if (vertCount > 0 && faceCount > 0) {
        ostringstream out;
        out << vertCount << " vertices, " << faceCount << " faces";
        return string(out.str());
    }
    
    if (scene->getModelContainers().size() > 0) {
        for (vector<ModelContainer*>::iterator it = scene->getModelContainers().begin(); it != scene->getModelContainers().end(); it++)
        {
            vertCount += (*it)->verticesCount();
            faceCount += (*it)->facesCount();
        }
    }
    
    return "---";
}
