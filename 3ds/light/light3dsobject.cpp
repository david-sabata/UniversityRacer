#include "light3dsobject.h"

using namespace std;

Light3DSObject::Light3DSObject(Model3DSChunk c)
{
	position.x = c.Float();
	position.y = c.Float();
	position.z = c.Float();
}

Light3DSObject::~Light3DSObject()
{
}


const Light3DSObject::POSITION &Light3DSObject::Position()
{
	return position;
}