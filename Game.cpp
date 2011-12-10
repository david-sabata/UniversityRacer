#include "Game.h"
#include "Light.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


#define WALK_SPEED 0.01f
#define STATICS_SCALE 0.05f


////////////////////////////////////////////////////////////////////////////////


Game::Game(): mouseCaptured(false), /*drawingQueue(NULL),*/ drawWireframe(false), followCamera(true)
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
	BaseModel* plank =  container->load3DS("models/plank.3ds");
    BaseModel* checkpoint =  container->load3DS("models/checkpoint.3ds");

    cout << "- initializing physics" << endl;

    physics = new Physics();
    physics->AddCar(PhysicsUtils::btTransFrom(btVector3(37.2f, 9.5f, -21.7f), btQuaternion(btVector3(0, 1, 0), -M_PI/2.f))); // 0,2,5
   //physics->AddRigidBody(5., PhysicsUtils::btTransFrom(btVector3(0, 3, 1)), new btBoxShape(btVector3(0.75,0.75,0.75)))->setAngularVelocity(btVector3(1,1,1)); // TODO konstruktor se neprelozi kvuli Debug.h    
    		
	cout << "- setting up drawing queue" << endl;
		
	// vykresli E112
	if (1) {
		container->addModel("e112", e112);
		glm::mat4 modelmat = glm::scale(glm::vec3(STATICS_SCALE));
		container->queueDraw(e112, modelmat);

        physics->AddStaticModel(Physics::CreateStaticCollisionShapes(e112, STATICS_SCALE), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false);
	}

    // vykresli zidle
	{
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
	{
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
			container->queueDraw(middesk, col);

            physics->AddStaticModel(middeskShapes, PhysicsUtils::btTransFrom(glm::scale(col, glm::vec3(1/STATICS_SCALE))), false);
		}
	}

	// vykresli postranni lavice
	{
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
		}
	}

    {
		container->addModel("car", car);
		carQueueItem = container->queueDraw(car);
	}

    {
		container->addModel("wheel", wheel);
		for (unsigned int i = 0; i < 4; i++)
            wheelQueueItem[i] = container->queueDraw(wheel);
	}

	// pridat desticky
	{        
		container->addModel("plank", plank);
        std::vector<btCollisionShape*> plankShapes = Physics::CreateStaticCollisionShapes(plank);

        //deska spojujici stoly nahore v pravo 1
		glm::mat4 pos = glm::translate(glm::vec3(20.8f, 8.62f, -21.8f));
	    glm::mat4 scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		glm::mat4 rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006f,0.0006f,0.009f));
		
		//deska v levo jede se na zem z horniho stolu
		pos = glm::translate(glm::vec3(-21.36f, 6.82f, -21.8f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.0115));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, -30.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.0115));
		
		//deska v v evo jede se smere dolu
		pos = glm::translate(glm::vec3(-30.36f, 4.20f, -18.9f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.006));


		//po sjeti o patro nize prejizdime pres MOST (v levo)
		pos = glm::translate(glm::vec3(-20.8f, 4.0f, -17.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.009));

		//deska uprosred  jede se smerem dolu ve smeru jizdy
		pos = glm::translate(glm::vec3(-6.36f, 3.19f, -13.95f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.006));


		//deska uprosred  jede se smerem dolu ve smeru jizdy - ploska 2
		pos = glm::translate(glm::vec3(-6.36f, 2.19f, -9.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.006));
		rot = glm::rotate(25.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.006));

		//deska v levo jede ze zeme nahoru na levy stul
		pos = glm::translate(glm::vec3(-19.5f, 3.50f, -7.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.015));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, 27.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.015));

		//ROURA DOLU MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-36.85f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0045,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-37.29f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-36.40f, 5.10f, -4.00f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//KONEC ROURA 1 MEZI ZIDLEMA


		
		//deska LEVO, SJEZD z LAVICE na LINO
		pos = glm::translate(glm::vec3(-27.85f, 2.071f, 7.61f));
	    scale = glm::scale(glm::vec3(0.007,0.0006,0.028));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.007,0.0006,0.028));

		//vyjezd za katedru
		pos = glm::translate(glm::vec3(-17.725f, 0.273f, 20.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, -15.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.009));

		//sjezd u katedry
		pos = glm::translate(glm::vec3(17.55f, 0.273f, 20.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, 15.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.009));


		//deska v pravo jede se smerem nahoru ve smeru jizdy - ploska 1
		pos = glm::translate(glm::vec3(35.36f, 0.273f, 2.08f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(15.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.006));


		//deska v pravo jede ze zeme nahoru na prosredni stul
		pos = glm::translate(glm::vec3(24.3f, 2.47f, -2.0f));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.020));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		rot = glm::rotate(rot, 20.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006,0.0006,0.020));


		//ROURA NAHORU 2 MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-10.85f, 5.072f, -4.017f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0045,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-11.29f, 5.072f, -4.017f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-10.41, 5.072f, -4.017f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//ROURA NAHORU 2 MEZI ZIDLEMA


		//ROURA NAHORU 3 MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-10.85f, 6.07f, -9.02f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0045,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-11.29f, 6.07f, -9.02f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(-10.41f, 6.07f, -9.02f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//ROURA NAHORU 3 MEZI ZIDLEMA


		//ROURA NAHORU 4 MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(4.9f, 7.075f, -14.023f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0045,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(5.34f, 7.075f, -14.023f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(4.46f, 7.075f, -14.023f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		// KONEC ROURA NAHORU 4 MEZI ZIDLEMA

		//deska spojujici stoly nahore v pravo 2 (ta niz)
		pos = glm::translate(glm::vec3(20.8f, 7.62f, -16.9));
	    scale = glm::scale(glm::vec3(0.006,0.0006,0.009));
		rot = glm::rotate(90.f, glm::vec3(0.f,1.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.006f,0.0006f,0.009f));

		//ROURA NAHORU 5 MEZI ZIDLEMA
		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(36.15f, 8.07f, -18.99f));
	    scale = glm::scale(glm::vec3(0.0045,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0045,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(36.59f, 8.07f, -18.99f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));

		//deska LEVO, SJEZD z LAVICE na LAVICI
		pos = glm::translate(glm::vec3(35.71f, 8.07f, -18.99f));
	    scale = glm::scale(glm::vec3(0.0009,0.0006,0.006));
		rot = glm::rotate(17.f, glm::vec3(1.f,0.f,0.f));
		rot = glm::rotate(rot, 90.f, glm::vec3(0.f,0.f,1.f));
		container->queueDraw(plank, pos * rot * scale);
        physics->AddStaticModel(plankShapes, PhysicsUtils::btTransFrom(pos * rot), true, btVector3(0.0009,0.0006,0.006));
		//KONEC ROURA 5 NAHORU


	}

    // vykresli checkpointy
    {
        container->addModel("checkpoint", checkpoint);
        
        // prvni checkpoint
        glm::mat4 mat = glm::rotate(glm::translate(glm::vec3(30.8f, 9.02f, -21.8f)), -90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));
        
        // druhy checkpoint
        mat = glm::rotate(glm::translate(glm::vec3(-10.8f, 9.02f, -21.8f)), -90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

        // treti checkpoint
        mat = glm::rotate(glm::translate(glm::vec3(-30.36f, 5.43f, -20.7f)), 0.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		// ctvrty checkpoint
        mat = glm::rotate(glm::translate(glm::vec3(-6.36f, 4.43f, -15.8f)), 0.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

        // paty checkpoint
        mat = glm::rotate(glm::translate(glm::vec3(-6.36f, 3.43f, -10.9f)), 0.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		// sesty checkpoint
        mat = glm::rotate(glm::translate(glm::vec3(-16.36f, 2.41f, -7.f)), -90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//sedmy checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(-36.85f, 5.99f, -5.82f)), 0.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//osmy checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(-27.85f, 4.99f, -0.8f)), 0.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//devaty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(-26.85f, 0.25f, 18.5f)), 45.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//desaty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(-18.725f, 0.25f, 20.0f)), 90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

	    //jedenacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(14.725f, 1.25f, 20.0f)), 90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//dvanacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(35.36f, 0.25f, 3.1f)), 180.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//trinacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(28.4f, 1.35f, -2.0f)), -90.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//ctrnacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(-10.85f, 5.f, -2.3f)), 180.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//patnacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(4.9f, 7.0f, -12.256f)), 180.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));

		//sestnacty checkpoint
		mat = glm::rotate(glm::translate(glm::vec3(36.15f, 8.0f, -17.0f)), 180.f, glm::vec3(0, 1, 0)); 
        container->queueDraw(checkpoint, glm::scale(mat, glm::vec3(CHECKPOINT_SCALE)));
        physics->Checkpoint().Add(PhysicsUtils::btTransFrom(mat));
		
		
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

    pos.left = Gui::RIGHT;
    guiCheckpoint = gui->addString(".", pos);

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
        
        if (i == PhysicsCar::WHEEL_FRONTRIGHT || i == PhysicsCar::WHEEL_REARRIGHT)
            wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

        container->updateDrawingMatrix(wheelQueueItem[i], wheelMatrix);
    }
	
	// vykreslit scenu
	scene->draw();

    if (drawWireframe)
    {
        // vykreslit fyziku
        physics->DebugDrawWorld();

        drawLines(physics->GetDebugDrawer()->GetLines());
    }
    
    // ---------------------------------------
	// Vykresleni ingame gui
	
	ostringstream time;
    time << physics->GetCar()->GetVehicle()->getCurrentSpeedKmHour(); //gameTime.Total();
    gui->updateString(guiTime, time.str());

    ostringstream checkpoint;
    checkpoint << "Checkpoint:" << physics->Checkpoint().PassedNum();
    gui->updateString(guiCheckpoint, checkpoint.str());
    

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
        physics->GetCar()->Brake();
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
        if (physics->Checkpoint().FirstPassed())
            physics->GetCar()->Reset(physics->Checkpoint().GetLastTrans());
        else
            physics->GetCar()->Reset(physics->GetCar()->GetInitialTransform());         

        camera.ResetFollow();
    }

    if (key == SDLK_r)
    {
        physics->Checkpoint().Reset();
        physics->GetCar()->Reset(physics->GetCar()->GetInitialTransform());
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
