#include "ShaderManager.h"

using namespace std;

// init statickych promennych
ShaderManager::MATERIAL ShaderManager::currentMaterial;
map<string, ShaderManager::MATERIAL> ShaderManager::materials = map<string, ShaderManager::MATERIAL>();





void ShaderManager::loadMaterial(string material)
{
	MATERIAL mat;

	// VS
	string vsPath = "materials/" + material + ".vert";
	string vsSource = loadFile(vsPath.c_str());
	GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource.c_str());

	// FS
	string fsPath = "materials/" + material + ".frag";
	string fsSource = loadFile(fsPath.c_str());
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource.c_str());

	// Program	
	mat.program = linkShader(2, vs, fs);

	// Vstupy
	mat.positionAttrib = glGetAttribLocation(mat.program, "position");
	mat.texposAttrib = glGetAttribLocation(mat.program, "texpos");
	mat.mvpUniform = glGetUniformLocation(mat.program, "mvp");

	materials[material] = mat;
}



ShaderManager::MATERIAL ShaderManager::useMaterial(string material)
{	
	map<string, MATERIAL>::iterator el = materials.find(material);
	
	if (el == materials.end()) 
		throw std::runtime_error("Material '" + material + "' is not loaded");

	MATERIAL mat = (*el).second;

	// nastavi aktualni material v manageru
	currentMaterial = mat;

	// a v GL
	glUseProgram(mat.program);
	
	return mat;
}


ShaderManager::MATERIAL ShaderManager::getCurrentMaterial()
{
	return currentMaterial;
}












//////////////////////////////////////////////////////////////////////
// Kod ze cviceni
//////////////////////////////////////////////////////////////////////


// Load whole file and return it as std::string
string ShaderManager::loadFile(const char * const file)
{
    std::ifstream stream(file);
    if(stream.fail()) throw std::runtime_error(std::string("Can't open \'") + file + "\'");
    return std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
}

// Info log contains errors and warnings from shader compilation
string ShaderManager::getShaderInfoLog(const GLuint shader)
{
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(length, ' ');
    glGetShaderInfoLog(shader, length, NULL, &log[0]);
    return log;
}

// Info log contains errors and warnings from shader linking
string ShaderManager::getProgramInfoLog(const GLuint program)
{
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::string log(length, ' ');
    glGetProgramInfoLog(program, length, NULL, &log[0]);
    return log;
}

GLuint ShaderManager::compileShader(const GLenum type, const char * source)
{
    GLuint shader = glCreateShader(type);
    if(shader == 0) throw GL_Exception("glCreateShader failed");

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    std::cout << "Compile: " << getShaderInfoLog(shader) << endl;

    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) throw std::runtime_error("shader compilation failed");

    return shader;
}

GLuint ShaderManager::linkShader(size_t count, ...)
{
    // Create program object
    GLuint program = glCreateProgram();
    if(program == 0) throw GL_Exception("glCreateProgram failed");

    // Attach arguments
    va_list args;
    va_start(args, count);
    for(size_t i = 0; i < count; ++i)
    {
        glAttachShader(program, va_arg(args, GLuint));
    }
    va_end(args);

    // Link program and check for errors
    glLinkProgram(program);
    std::cout << "Link: " << getProgramInfoLog(program) << endl;

    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE) throw std::runtime_error("shader linking failed");

    return program;
}