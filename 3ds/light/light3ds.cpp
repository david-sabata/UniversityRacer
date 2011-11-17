#include "light3ds.h"

using namespace std;

Light3DS::Light3DS()
{
}
Light3DS::~Light3DS()
{
}

void Light3DS::Parse(const string &name , Model3DSChunk c)
{	
	Light3DSObject light(c);
	lights.push_back(light);
}

const vector<Light3DSObject> &Light3DS::Lights()
{
	return lights;
}