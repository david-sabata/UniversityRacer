#include "file.h"

#include <iostream>
using namespace std;

Model3DSFile::Model3DSFile(const char *src)
	: file(src, ifstream::binary | ifstream::in)
{
}
Model3DSFile::~Model3DSFile()
{
	//cout << "Model3DSFile::~Model3DSFile()\n";
	file.close();
}

Model3DSChunk Model3DSFile::Child()
{
	file.seekg(0);
	return Model3DSChunk(file , FileSize(file));
}

int Model3DSFile::FileSize(ifstream &file)
{
	int curr_pos = (int)file.tellg();

	file.seekg(0);
	int beginning = (int)file.tellg();
	file.seekg(0 , ios::end);
	int ending = (int)file.tellg();

	file.seekg(curr_pos);

	return ending - beginning;	// ;)
}
