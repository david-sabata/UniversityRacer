#ifndef LIGHT3DSOBJECT_H
#define LIGHT3DSOBJECT_H

#include <string>
#include "../chunk/chunk.h"

/***/
class Light3DSObject
{
    public:

		typedef struct Position {
			float x;
			float y;
			float z;
		} POSITION;

		/***/
		Light3DSObject(Model3DSChunk c);
		/***/
		~Light3DSObject();

		const POSITION &Position();
    
	private:
		POSITION position;
};

#endif
