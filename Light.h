#ifndef LIGHT_H
#define LIGHT_H


class Light {
	public:
		
		typedef struct {
			float x, y, z;
		} POSITION;



		Light(float x, float y, float z);

		POSITION &Position();

	public: //zmeni na protected
		POSITION position;
};


#endif
