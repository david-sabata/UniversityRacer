#include "Game.h"
#include "Light.h"

using namespace std;


#define WALK_SPEED 0.1f

// pole pomocnych car k vykresleni - caru definuji dva body a barva
struct LINE {
	LINE(glm::vec3 a, glm::vec3 b, glm::vec3 color) : a(a), b(b), color(color) {};

	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 color;
};
vector<LINE> lines;

GLuint linesVBO;
GLuint linesEBO;


////////////////////////////////////////////////////////////////////////////////


Game::Game(): mouseCaptured(false), drawingQueue(NULL), drawWireframe(false)
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

#if 1
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
		glm::mat4 rows[] = {
			glm::translate(scale, glm::vec3(-740, 19, -70)),
			glm::translate(scale, glm::vec3(-740, 39, -170)),
			glm::translate(scale, glm::vec3(-740, 59, -270)),
			glm::translate(scale, glm::vec3(-740, 79, -370)),
			glm::translate(scale, glm::vec3(-740, 99, -470))
		};

		container->addModel("chairs", chairs);
		
		for (unsigned int rowI = 0; rowI < 5; rowI++)
		{
			int offsetX = 0; // posunuti zidle na radku

			for (unsigned int i = 0; i < 13; i++)
			{				
				if (i == 3 || i == 10)
					offsetX += 100;

				glm::mat4 col = glm::translate(rows[rowI], glm::vec3(offsetX, 0, 0));
				container->queueDraw(chairs, col);
			
				offsetX += 105;
			}		
		}
	}

	// pro kazde svetlo v kontejneru pridat kouli, ktera ho znazornuje
	if (0) {
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
#endif

	cout << "- constructing scene" << endl;
	
	// vyrobit scenu
	scene = new Scene(*this);
	scene->addModelContainer(container);
	scene->init();
	
	cout << "- done!" << endl;
	
	// nacist vsechny materialy	
	ShaderManager::loadPrograms();


	ShaderManager::loadProgram("line");

	drawLine(glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), glm::vec3(1, 1, 1));	
}
 



void Game::onWindowRedraw(const GameTime & gameTime) 
{	
	BaseApp::onWindowRedraw(gameTime);

	handleActiveKeys(gameTime);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);    
	glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
	
	// vykreslit scenu
	scene->draw();


#if 0
	// vykresleni car -----------------------
	glDisable(GL_CULL_FACE);

	vector<GLfloat> vertices;
	vector<GLuint> indices;
	unsigned int indexI = 0;

	for (vector<LINE>::iterator it = lines.begin(); it != lines.end(); it++)
	{
		LINE l = (*it);
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
	glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)&(vertices.begin()));

	// modelova matice
	glm::mat4 modelmat = glm::mat4(1.0);
	glUniformMatrix4fv(activeBinding.mModelUniform, 1, GL_FALSE, glm::value_ptr(modelmat));

	// pohledova matice
	glm::mat4 mView = getCamera()->GetMatrix();
	glUniformMatrix4fv(activeBinding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));

	// projekcni matice
	glm::mat4 mProjection = glm::perspective(45.0f, (float)getWindowAspectRatio(), 1.0f, 1000.0f);
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
#endif
	

	// ---------------------------------------

    SDL_GL_SwapBuffers(); 
}





void Game::drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color)
{
	LINE l(a, b, color);
	lines.push_back(l);
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

