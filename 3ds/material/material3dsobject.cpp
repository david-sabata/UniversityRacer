#include "material3dsobject.h"

using namespace std;

Material3DSObject::Material3DSObject(Model3DSChunk c)
{
	for(Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
	{
		switch(cc.ID())
		{
			case(0xA000):
				name = cc.Str();
			break;

			case (0xA040):
				for (Model3DSChunk ccc = cc.Child(); ccc; ccc = ccc.Sibling())
				{
					if (ccc.ID() == 0x0031)
					{
						shininess = ccc.Int();
						break;
					}
					if (ccc.ID() == 0x0030)
					{
						shininess = ccc.Short();
						break;
					}
				}				
			break;

			case(0xA010):				
				for (Model3DSChunk ccc = cc.Child(); ccc; ccc = ccc.Sibling())
				{
					switch(ccc.ID())
					{
						case (0x0010):
							ambientColor.r = ccc.Float();
							ambientColor.g = ccc.Float();
							ambientColor.b = ccc.Float();
						break;
						case (0x0011):
							ambientColor.r = (float)(ccc.Char() / (float)255);
							ambientColor.g = (float)(ccc.Char() / (float)255);
							ambientColor.b = (float)(ccc.Char() / (float)255);
						break;
					}
				}				
			break;

			case(0xA020):
				for (Model3DSChunk ccc = cc.Child(); ccc; ccc = ccc.Sibling())
				{
					switch(ccc.ID())
					{
						case (0x0010):
							diffuseColor.r = ccc.Float();
							diffuseColor.g = ccc.Float();
							diffuseColor.b = ccc.Float();
						break;
						case (0x0011):
							diffuseColor.r = (float)(ccc.Char() / (float)255);
							diffuseColor.g = (float)(ccc.Char() / (float)255);
							diffuseColor.b = (float)(ccc.Char() / (float)255);
						break;
					}
				}
			break;

			case(0xA030):
				for (Model3DSChunk ccc = cc.Child(); ccc; ccc = ccc.Sibling())
				{
					switch(ccc.ID())
					{
						case (0x0010):
							specularColor.r = ccc.Float();
							specularColor.g = ccc.Float();
							specularColor.b = ccc.Float();
						break;
						case (0x0011):
							specularColor.r = (float)(ccc.Char() / (float)255);
							specularColor.g = (float)(ccc.Char() / (float)255);
							specularColor.b = (float)(ccc.Char() / (float)255);
						break;
					}
				}
			break;
		}
	}
}
Material3DSObject::~Material3DSObject()
{
}

const std::string &Material3DSObject::Name()
{
	return name;
}
const std::string &Material3DSObject::Texture()
{
	return name;
}
const Mesh3Dcolor &Material3DSObject::AmbientColor()
{
	return ambientColor;
}
const Mesh3Dcolor &Material3DSObject::DiffuseColor()
{
	return diffuseColor;
}
const Mesh3Dcolor &Material3DSObject::SpecularColor()
{
	return specularColor;
}
const int Material3DSObject::Shininess()
{
	return shininess;
}