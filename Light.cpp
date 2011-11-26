#include "Light.h"



Light::Light(float x, float y, float z) 
{
	position.x = x;
	position.y = y;
	position.z = z;
}


const Light::POSITION &Light::Position()
{
	return position;
}