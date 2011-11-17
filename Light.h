#ifndef LIGHT_H
#define LIGHT_H


class Light {
	public:
		
		typedef struct {
			float x, y, z;
		} POSITION;



		Light(float x, float y, float z);

		const POSITION &Position();

	protected:
		POSITION position;
};


#endif