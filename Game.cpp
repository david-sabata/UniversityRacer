#include "Game.h"


struct Point {
    float color[3];
    float position[3];
} const houseVertices[] = {
    // Walls
    { { 0.0, 0.0, 1.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 0.0 }, { -5.0, -5.0,  5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, {  5.0, -5.0, -5.0 } },
        
    { { 1.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 0.0, 1.0 }, {  5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, { -5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0 }, {  5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    // Roof
    { { 0.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }, 

    { { 1.0, 0.0, 0.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 1.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 0.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }
};

// House indices
const unsigned char house[] = {
//Walls
     0,  1,  2,
     0,  2,  3,
     4,  5,  6,
     4,  6,  7,
     8,  9, 10,
     8, 10, 11,
    12, 13, 14,
    12, 14, 15,
//Roof
    16, 17, 18,
    19, 20, 21,
    22, 23, 24,
    25, 26, 27
};

GLuint VBO, EBO;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec3 position; in vec3 color; uniform mat4 mvp; out vec3 c; void main() { gl_Position = mvp*vec4(position,1); c = color; }";
const char * FSSource
    = "#version 130\n in vec3 c; out vec4 FragColor; void main() { FragColor = vec4(c,1); }";

GLuint VS, FS, Prog;

GLuint positionAttrib, colorAttrib, mvpUniform;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Game::onInit()
{
    // Shader
    VS = CompileShader(GL_VERTEX_SHADER, VSSource);
    FS = CompileShader(GL_FRAGMENT_SHADER, FSSource);
    Prog = LinkShader(2, VS, FS);

    positionAttrib = glGetAttribLocation(Prog, "position");
    colorAttrib = glGetAttribLocation(Prog, "color");
    mvpUniform = glGetUniformLocation(Prog, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);
}
 



void Game::onWindowRedraw() {	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    
    glDepthFunc(GL_LESS);

    glUseProgram(Prog);

    //MVP
    glm::mat4 projection;

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

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, sizeof(house)/sizeof(*house), GL_UNSIGNED_BYTE, NULL);

    SDL_GL_SwapBuffers(); 
}