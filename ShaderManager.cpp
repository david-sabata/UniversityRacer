#include "ShaderManager.h"

using namespace std;

// init statickych promennych
ShaderManager::MATERIAL ShaderManager::currentMaterial;
map<string, ShaderManager::MATERIAL> ShaderManager::materials = map<string, ShaderManager::MATERIAL>();
map<string, GLuint> ShaderManager::textures = map<string, GLuint>();


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
	mat.normalAttrib = glGetAttribLocation(mat.program, "normal");
	mat.texposAttrib = glGetAttribLocation(mat.program, "texpos");
	mat.mViewUniform = glGetUniformLocation(mat.program, "view");
	mat.mProjectionUniform = glGetUniformLocation(mat.program, "projection");
	mat.mModelUniform = glGetUniformLocation(mat.program, "model");

	// Nacist textury
	mat.textures = loadTextures(mat.program, vsSource + fsSource);
	
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
	

	// aktivuje prislusne textury; predpokladame podporu alespon 6 textur
	GLenum textureEnums[] = { GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5 };

	// postupne vse nastavit; pri samotnem kresleni jiz neni treba
	for (unsigned int i = 0; i < mat.textures.size(); i++)
	{		
		TEXTUREBINDING binding = mat.textures[i];

		glActiveTexture(textureEnums[i]);		
		glBindTexture(GL_TEXTURE_2D, binding.texture);
		
		// nastaveni parametru sampleru
				
		if (binding.WRAP_S == GL_REPEAT || binding.WRAP_S == GL_CLAMP || binding.WRAP_S == GL_CLAMP_TO_EDGE)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, binding.WRAP_S);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		if (binding.WRAP_T == GL_REPEAT || binding.WRAP_T == GL_CLAMP || binding.WRAP_T == GL_CLAMP_TO_EDGE)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, binding.WRAP_T);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (binding.MAG_FILTER == GL_NEAREST || binding.MAG_FILTER == GL_LINEAR || binding.MAG_FILTER == GL_NEAREST_MIPMAP_LINEAR ||
			binding.MAG_FILTER == GL_NEAREST_MIPMAP_NEAREST || binding.MAG_FILTER == GL_LINEAR_MIPMAP_LINEAR || 
			binding.MAG_FILTER == GL_LINEAR_MIPMAP_NEAREST || binding.MAG_FILTER == GL_TEXTURE_MIN_LOD || 
			binding.MAG_FILTER == GL_TEXTURE_MAX_LOD || binding.MAG_FILTER == GL_TEXTURE_BASE_LEVEL || binding.MAG_FILTER == GL_TEXTURE_MAX_LEVEL)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, binding.MAG_FILTER);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (binding.MIN_FILTER == GL_NEAREST || binding.MIN_FILTER == GL_LINEAR || binding.MIN_FILTER == GL_NEAREST_MIPMAP_LINEAR ||
			binding.MIN_FILTER == GL_NEAREST_MIPMAP_NEAREST || binding.MIN_FILTER == GL_LINEAR_MIPMAP_LINEAR || 
			binding.MIN_FILTER == GL_LINEAR_MIPMAP_NEAREST || binding.MIN_FILTER == GL_TEXTURE_MIN_LOD || 
			binding.MIN_FILTER == GL_TEXTURE_MAX_LOD || binding.MIN_FILTER == GL_TEXTURE_BASE_LEVEL || binding.MIN_FILTER == GL_TEXTURE_MAX_LEVEL)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, binding.MIN_FILTER);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glUniform1i(binding.uniform, i);
	}
	
	return mat;
}


ShaderManager::MATERIAL ShaderManager::getCurrentMaterial()
{
	return currentMaterial;
}





vector<ShaderManager::TEXTUREBINDING> ShaderManager::loadTextures(GLuint program, string source)
{
	vector<string> lines;
	istringstream stream(source);
	
	// rozsekat vstup na radky
	while (stream.good()) 
	{
		string s;
		getline(stream, s);
		lines.push_back(s);
	}

	vector<TEXTUREBINDING> bindings;
	
	
	TEXTUREBINDING binding;
	binding.isValid = false;

	// budeme postupne nacitat radky,
	// pokud je radek anotace, naplnime ji strukturu,
	// pokud je deklarace, vezmeme strukturu a ulozime,
	// jinak strukturu zahodime
	for (unsigned int i = 0; i < lines.size(); i++) 
	{
		// anotace
		size_t annot = lines[i].find("// @");
		if (annot != string::npos)
		{
			string tmp = lines[i].substr(4); // preskoci '// @'
			size_t space = tmp.find_first_of(' '); // mezera mezi nazvem anotace a hodnotou
			string anName = tmp.substr(0, space); // nazev anotace
			tmp.erase(0, space + 1); // odmaze vse pred hodnotou anotace
			size_t eol = tmp.find_first_of(" \t\f\v\n\r"); // prvni prazdny znak za hodnotou
			string anValue = tmp.substr(0, eol);

			// nacitame texturu?
			if (anName == "LOAD")
			{
				SDL_Surface * surface = SDL_LoadBMP(anValue.c_str());
				if(surface == NULL) throw SDL_Exception();

				glGenTextures(1, &binding.texture);
				glBindTexture(GL_TEXTURE_2D, binding.texture);
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automaticke mipmapy pro starsi gl
				SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);								

				binding.isValid = true;

				continue; // dal tuto anotaci neresit
			}

			// dohledame nastavovany GL parametr
			GLint* param;			
			if (anName == "MIN_FILTER") param = &binding.MIN_FILTER;
			else if (anName == "MAG_FILTER") param = &binding.MAG_FILTER;
			else if (anName == "WRAP_S") param = &binding.WRAP_S;
			else if (anName == "WRAP_T") param = &binding.WRAP_T;
			else
			{
				cerr << "Error: Unknown shader annotation '" << anName << "'" << endl;
				return vector<TEXTUREBINDING>();
			}
			
			// dohledame nastavovanou GL hodnotu
			if (anValue == "NEAREST") *param = GL_NEAREST;
			else if (anValue == "LINEAR") *param = GL_LINEAR;
			else if (anValue == "NEAREST_MIPMAP_NEAREST") *param = GL_NEAREST_MIPMAP_NEAREST;
			else if (anValue == "LINEAR_MIPMAP_NEAREST") *param = GL_LINEAR_MIPMAP_NEAREST;
			else if (anValue == "NEAREST_MIPMAP_LINEAR") *param = GL_NEAREST_MIPMAP_LINEAR;
			else if (anValue == "LINEAR_MIPMAP_LINEAR") *param = GL_LINEAR_MIPMAP_LINEAR;
			else if (anValue == "TEXTURE_MIN_LOD") *param = GL_TEXTURE_MIN_LOD;
			else if (anValue == "TEXTURE_MAX_LOD") *param = GL_TEXTURE_MAX_LOD;
			else if (anValue == "TEXTURE_BASE_LEVEL") *param = GL_TEXTURE_BASE_LEVEL;
			else if (anValue == "TEXTURE_MAX_LEVEL") *param = GL_TEXTURE_MAX_LEVEL;
			else if (anValue == "CLAMP") *param = GL_CLAMP;
			else if (anValue == "CLAMP_TO_EDGE") *param = GL_CLAMP_TO_EDGE;
			else if (anValue == "REPEAT") *param = GL_REPEAT;
			else 
			{
				cerr << "Error: Unknown shader annotation value '" << anValue << "'" << endl;
				return vector<TEXTUREBINDING>();
			}

			continue; // dal tento radek neresit
		} // anotace

		// deklarace
		size_t decl = lines[i].find("uniform sampler2D ");
		if (decl != string::npos)
		{
			string identifier = lines[i].substr( string("uniform sampler2D ").size() );
			size_t empty = identifier.find_first_of("; \t\f\v\n\r"); // pozice prvniho pripadneho prazdneho znaku za idtf.
			if (empty != string::npos)
				identifier.erase(empty);

			binding.uniform = glGetUniformLocation(program, identifier.c_str()); // ulozime handle

			// pokud byla definovana platna textura (anotace @load je povinna), ulozime binding
			if (binding.isValid)
				bindings.push_back(binding);											
			// neuplna anotace pred deklaraci vypise pouze varovani (neukonci)
			else
				cerr << "Warning: incomplete annotation near '" << identifier << "'" << endl;

			// novy binding pro pristi radky
			binding = TEXTUREBINDING();
			binding.isValid = false;

			continue;
		} // deklarace


		// na radku neni ani anotace, ani deklarace; jestlize existovala nejaka nepouzita anotace, zneplatnime ji
		binding = TEXTUREBINDING();
		binding.isValid = false;
	} // for each line


#if 0
	for (unsigned int i = 0; i < bindings.size(); i++)
	{
		TEXTUREBINDING b = bindings[i];
		cout << "uniform: " << b.uniform << endl << "texture: " << b.texture << endl << "min_f: " << b.MIN_FILTER << endl;
		cout << "mag_f: " << b.MAG_FILTER << endl << "wrap_s: " << b.WRAP_S << endl << "wrap_t: " << b.WRAP_T << endl;
		cout << "---------------------------------------" << endl;
	}
#endif

	return bindings;
}










//////////////////////////////////////////////////////////////////////
// Kod ze cviceni
//////////////////////////////////////////////////////////////////////

// Nacte obrazek do textury
void ShaderManager::SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface)
{
	// nejdriv vertikalne prevratit, at ma obrazek stejne souradnice jako GL
	verticalSurfaceFlip(surface);

    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    if(     (surface->format->Rmask == 0xff0000) &&
            (surface->format->Gmask == 0xff00) &&
            (surface->format->Bmask == 0xff) &&
            (surface->format->Amask == 0))
    {
        glTexImage2D(target, level, internalformat, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else if((surface->format->Rmask == 0xff) &&
            (surface->format->Gmask == 0xff00) &&
            (surface->format->Bmask == 0xff0000) &&
            (surface->format->Amask == 0))
    {
        glTexImage2D(target, level, internalformat, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else
    {
        throw std::runtime_error("unsupported surface format");
    }
}

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



////////////////////////////////////////////////////////////////////
// Kod z webu pro horizontalni flip SDL_surface, resp. textury
////////////////////////////////////////////////////////////////////

/**
 * http://www.libsdl.org/docs/html/guidevideo.html
 */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}	

/**
 * http://www.libsdl.org/docs/html/guidevideo.html
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


void ShaderManager::verticalSurfaceFlip(SDL_Surface*& image)
{
	// create a copy of the image    
	SDL_Surface* flipped_image = SDL_CreateRGBSurface( SDL_SWSURFACE, image->w, image->h, image->format->BitsPerPixel,
		image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask );
	
	// loop through pixels    
	for ( int y=0; y<image->h; y++ ) {
		for ( int x=0; x<image->w; x++ ) {            
			// copy pixels, but reverse the y pixels!
			putpixel( flipped_image, x, y, getpixel(image, x, image->h - y -1) );        
			//putpixel( flipped_image, x, y, getpixel(image, image->w - x - 1, y) );        
		}    
	}    
	// free original and assign flipped to it    
	SDL_FreeSurface( image );    
	image = flipped_image;
}