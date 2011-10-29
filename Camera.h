#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include <glm/gtx/transform2.hpp>

#include "Utils.h"



class Camera {

	public:

		Camera(void);
		void Reset();
		
		void Move(float x, float y, float z);		
		void Aim(float vertical_angle, float horizontal_angle);
		
		glm::mat4 GetMatrix();		

		void DebugDump();

		//void toggleObserve();

		inline glm::vec3 getEye()
		{
			return eye;
		}
		
		inline glm::vec3 getTarget()
		{
			return target;
		}

	private:
		glm::vec3 eye;			// pozice kamery
		glm::vec3 target;		// smer pohledu
		glm::vec3 up;			// up vektor kamery
		float angle_horiz;
		float angle_vert;

		/*
		bool isObserving;
		Vector3f observeCenter;
		float observeRadius;
		float observeSpeed;
		CTimer observeTimer;
		*/	

};

#endif