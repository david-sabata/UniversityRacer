#include "Game.h"
#include "Light.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


#define WALK_SPEED 0.01f
#define STATICS_SCALE 0.05f


////////////////////////////////////////////////////////////////////////////////


Game::Game(): mouseCaptured(false), /*drawingQueue(NULL),*/ drawWireframe(false), followCamera(false)
{
	gui = new Gui(windowWidth, windowHeight);
	scene = new Scene(*this);
	shadowVolumes = new ShadowVolumes();
}

Game::~Game()
{
	delete gui;
	delete scene;
	delete shadowVolumes;
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

	// modely pro kresleni
	BaseModel* chairs = container->load3DS("models/chairs.3ds");
	BaseModel* e112 = container->load3DS("models/e112.3ds");
	BaseModel* middesk = container->load3DS("models/desk-mid.3ds");	
  	BaseModel* sidedesk = container->load3DS("models/desk-side.3ds");
    BaseModel* car =  container->load3DS("models/car.3ds");
    BaseModel* wheel =  container->load3DS("models/wheel.3ds");
	BaseModel* plank =  container->load3DS("models/plank.3ds");

    cout << "- initializing physics" << endl;

    physics = new Physics();
	physics->AddCar(PhysicsUtils::btTransFrom(btVector3(37.19f, 9.5f, -21.7f), btQuaternion(btVector3(0, 1, 0), btScalar(-M_PI/2.f)))); // 0,2,5
   //physics->AddRigidBody(5., PhysicsUtils::btTransFrom(btVector3(0, 3, 1)), new btBoxShape(btVector3(0.75,0.75,0.75)))->setAngularVelocity(btVector3(1,1,1)); // TODO konstruktor se neprelozi kvuli Debug.h    
    		
	cout << "- setting up drawing queue" << endl;
		
	// vykresli E112
	if (1) {
		container->addModel("e112", e112);
		glm::mat4 modelmat = glm::scale(glm::vec3(STATICS_SCALE));
		e112QueueItem = container->queueDraw(e112, modelmat);

        physics->AddStaticModel(Physics::CreateStaticCollisionShapes(e112, STATICS_SCALE), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false);
	}

    // vykresli zidle
	if (0) {
		container->addModel("chairs", chairs);
                
		glm::mat4 scale = glm::scale(glm::vec3(STATICS_SCALE));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-740, 19, -70)),
			glm::translate(scale, glm::vec3(-740, 39, -170)),
			glm::translate(scale, glm::vec3(-740, 59, -270)),
			glm::translate(scale, glm::vec3(-740, 79, -370)),
			glm::translate(scale, glm::vec3(-740, 99, -470))
		};

        std::vector<btCollisionShape*> chairShapes = Physics::CreateStaticCollisionShapes(chairs, STATICS_SCALE);
		
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

                physics->AddStaticModel(chairShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/STATICS_SCALE))), false);
			}		
		}
	}

	// vykresli prostredni lavice
	if (1) {
		container->addModel("middesk", middesk);

		glm::mat4 scale = glm::scale(glm::vec3(STATICS_SCALE));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-365, 13, -43)),
			glm::translate(scale, glm::vec3(-365, 33, -143)),
			glm::translate(scale, glm::vec3(-365, 53, -243)),
			glm::translate(scale, glm::vec3(-365, 73, -343)),
			glm::translate(scale, glm::vec3(-365, 93, -443))
		};

        std::vector<btCollisionShape*> middeskShapes = Physics::CreateStaticCollisionShapes(middesk, STATICS_SCALE);

		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			glm::mat4 col = glm::translate(rows[rowI], glm::vec3(0, 0, 0));

            physics->AddStaticModel(middeskShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/STATICS_SCALE))), false);

			container->queueDraw(middesk, col);
			shadowVolumes->addModel(middesk, col);
		}
	}

	// vykresli postranni lavice
	if (1) {
		container->addModel("sidedesk", sidedesk);

		glm::mat4 scale = glm::scale(glm::vec3(STATICS_SCALE));
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-785,  20, -15)),
			glm::translate(scale, glm::vec3(-785,  40, -115)),
			glm::translate(scale, glm::vec3(-785,  60, -215)),
			glm::translate(scale, glm::vec3(-785,  80, -315)),
			glm::translate(scale, glm::vec3(-785, 100, -415))
		};

        std::vector<btCollisionShape*> sidedeskShapes = Physics::CreateStaticCollisionShapes(sidedesk, STATICS_SCALE);

		glm::vec3 otherside(1250, 0, 0);

		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			// leva strana (z pohledu z katedry)
			glm::mat4 col = glm::translate(rows[rowI], glm::vec3(0, 0, 0));			
			container->queueDraw(sidedesk, col);

            physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/STATICS_SCALE))), false);

			// prava strana
			glm::mat4 mat = glm::translate(col, otherside);
			container->queueDraw(sidedesk, mat);
			//shadowVolumes->addModel(sidedesk, mat);

            physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(glm::scale(mat, glm::vec3(1/STATICS_SCALE))), false);
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
			shadowVolumes->addLight(glm::vec3(pos));
		}
	}

	// pridat auto
    {
		container->addModel("car", car);
		carQueueItem = container->queueDraw(car);
	}

	// pridat kola auta
    {
		container->addModel("wheel", wheel);
		for (unsigned int i = 0; i < 4; i++)
        {
            wheelQueueItem[i] = container->queueDraw(wheel);
        }
	}

	// pridat desticky
	{        
		container->addModel("plank", plank);
        std::vector<btCollisionShape*> plankShapes1 = Physics::CreateStaticCollisionShapes(plank, btVector3(0.006f,0.0006f,0.009f));
        std::vector<btCollisionShape*> plankShapes2 = Physics::CreateStaticCollisionShapes(plank, btVector3(0.006f,0.0006f,0.0115f));
        std::vector<btCollisionShape*> plankShapes3 = Physics::CreateStaticCollisionShapes(plank, btVector3(0.006f,0.0006f,0.006f));

        //deska spojujici stoly nahore v pravo
		glm::mat4 pos = glm::translate(glm::vec3(20.8f, 8.62f, -21.8f));
	    glm::mat4 scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		glm::mat4 rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes1, PhysicsUtils::btTransFrom(pos * rot));
		
		//deska v levo jede se na zem z horniho stolu
		pos = glm::translate(glm::vec3(-21.36f, 6.82f, -21.8f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.0115));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, -30.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes2, PhysicsUtils::btTransFrom(pos * rot));
		
		//deska v v evo jede se smere dolu
		pos = glm::translate(glm::vec3(-30.36f, 4.20f, -18.9f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));


		//po sjeti o patro nize prejizdime pres MOST (v levo)
		pos = glm::translate(glm::vec3(-20.8f, 4.0f, -17.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes1, PhysicsUtils::btTransFrom(pos * rot));

		//deska uprosred  jede se smerem dolu ve smeru jizdy
		pos = glm::translate(glm::vec3(-3.36f, 3.19f, -13.95f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));


		//deska uprosred  jede se smerem dolu ve smeru jizdy - ploska 2
		pos = glm::translate(glm::vec3(-3.36f, 2.19f, -9.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));

		//deska v levo jede ze zeme nahoru na levy stul
		pos = glm::translate(glm::vec3(-19.5f, 3.50f, -7.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.015));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, 27.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes2, PhysicsUtils::btTransFrom(pos * rot));

		//ROURA DOLU MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-36.85f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-37.29f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));

				//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-36.40f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes3, PhysicsUtils::btTransFrom(pos * rot));

		//KONEC ROURA 1 MEZI ZIDLEMA
	}


	cout << "- constructing scene" << endl;

	// vyrobit scenu
	scene->addModelContainer(container);
	scene->init();

	cout << "- constructing shadow volumes" << endl;

	// vygenerovat stinova telesa
	shadowVolumes->generate();
	
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

	// fyzika -------------------------------------------------
    physics->StepSimulation(gameTime.Elapsed() * 0.001f);
    
    glm::mat4 carMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetWorldTransform()), glm::vec3(CAR_SCALE));

    container->updateDrawingMatrix(carQueueItem, carMatrix);

    if (followCamera)
    {
        btVector3 vel = physics->GetCar()->GetVehicle()->getRigidBody()->getLinearVelocity();        
        camera.Follow(carMatrix, glm::vec3(vel.x(), vel.y(), vel.z()), gameTime);
    }    
 
    for (int i = 0; i < physics->GetCar()->GetVehicle()->getNumWheels(); i++)
    {
        physics->GetCar()->GetVehicle()->updateWheelTransform(i, true); //synchronize the wheels with the (interpolated) chassis worldtransform        
        glm::mat4 wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(physics->GetCar()->GetVehicle()->getWheelInfo(i).m_worldTransform), glm::vec3(CAR_SCALE));
        
        if (i == CarPhysics::WHEEL_FRONTRIGHT || i == CarPhysics::WHEEL_REARRIGHT)
            wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

        container->updateDrawingMatrix(wheelQueueItem[i], wheelMatrix);
    }
	// ---------------------------------------------------------


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST); // Activate the depth test
    glEnable(GL_CULL_FACE); // Activate the culling
    glCullFace(GL_BACK);   // We are drawing front face
   
	scene->draw();

	shadowVolumes->draw(0, getCamera()->GetMatrix(), glm::perspective(45.0f, (float)getWindowAspectRatio(), 0.1f, 1000.0f));


#if 0
	// vicepruchodove kresleni se stiny podle:
	// http://www.angelfire.com/games5/duktroa/RealTimeShadowTutorial.htm
	// vykreslit scenu do z-bufferu ------------------------------	
#if 1
	glEnable(GL_DEPTH_TEST); // Activate the depth test
    glEnable(GL_CULL_FACE); // Activate the culling
    glCullFace(GL_BACK);   // We are drawing front face
    glDisable(GL_TEXTURE_2D); // no texture here
    glDisable(GL_BLEND);   // no blending
    glDepthMask(GL_TRUE);  // Writing on z-buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);  // No writing on color buffer

	scene->draw();

	glBlendFunc(GL_ONE, GL_ONE); // The blending function scr+dst, to add all the lighting
    glDepthMask(GL_FALSE);  // We stop writing to z-buffer now. We made this in the first pass, now we have it
    glEnable(GL_STENCIL_TEST); // We enable the stencil testing
#endif
	// -----------------------------------------------------------

	// vykreslit stiny; predpoklada se naplneny z-buffer ---------
#if 1
	glm::mat4 mView = getCamera()->GetMatrix();
	glm::mat4 mPerspective = glm::perspective(45.0f, (float)getWindowAspectRatio(), 1.0f, 1000.0f);
	
	unsigned int lightI = 0;
	
	glDisable(GL_BLEND); // We don't want lighting. We are only writing in stencil buffer for now
    glClear(GL_STENCIL_BUFFER_BIT); // We clear the stencil buffer
    glDepthFunc(GL_LESS); // We change the z-testing function to LESS, to avoid little bugs in shadow
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // We dont draw it to the screen
    glStencilFunc(GL_ALWAYS, 0, 0); // We always draw whatever we have in the stencil buffer

	glCullFace(GL_FRONT); // We are drawing the back faces first 
    glStencilOp(GL_KEEP, GL_INCR, GL_KEEP); // We increment if the depth test fails
	
	shadowVolumes->draw( lightI, mView, mPerspective );

	glCullFace(GL_BACK); // We are now drawing the front faces
    glStencilOp(GL_KEEP, GL_DECR, GL_KEEP); // We decrement if the depth test fails

	shadowVolumes->draw( lightI, mView, mPerspective );
#endif
	// -----------------------------------------------------------

	// vykreslit scenu normalne ----------------------------------
#if 1
	// We draw our lighting now that we created the shadows area in the stencil buffer
    glDepthFunc(GL_LEQUAL); // we put it again to LESS or EQUAL (or else you will get some z-fighting)
    glCullFace(GL_BACK); // we draw the front face
    glEnable(GL_BLEND); // We enable blending
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // We enable color buffer
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Drawing will not affect the stencil buffer
    glStencilFunc(GL_EQUAL, 0x0, 0xff); // And the most important thing, the stencil function. Drawing if equal to 0

	scene->draw();
#endif
	// -----------------------------------------------------------
#endif
	

    if (drawWireframe)
    {
        // vykreslit fyziku
        physics->DebugDrawWorld();

        drawLines(physics->GetDebugDrawer()->GetLines());
    }
    
    // ---------------------------------------
	// Vykresleni ingame gui
#if 1
	ostringstream time;
    time << physics->GetCar()->GetVehicle()->getCurrentSpeedKmHour(); //gameTime.Total();

	gui->updateString(guiTime, time.str());
	gui->draw();
#endif
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
