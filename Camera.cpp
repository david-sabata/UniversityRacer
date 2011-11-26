#include "Camera.h"

using namespace std;


Camera::Camera(void) {
	Reset();
}


void Camera::Reset() {
	//eye = glm::vec3(2.78f, 2.73f, -2.5f);	
	eye = glm::vec3(0.0f, 0.0f, -10.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);		
	target = glm::vec3(0.0f, 0.0f, 1.0f);
	angle_horiz = 0.0f;
	angle_vert = 0.0f;

	/*
	isObserving = false;
	observeCenter = Vector3f(0.0f, 0.0f, 0.0f);
	observeRadius = 3.0f;
	observeSpeed = 0.7f;
	observeTimer.ResetTimer();
	*/
}



void Camera::Move(float x, float y, float z) {
	
	// posunout o z-nasobek smeru pohledu
	glm::vec3 dir = glm::normalize(target);	
	eye += dir * z;

	// posunout do strany o x-nasobek right vektoru
	glm::vec3 r = glm::normalize(target);	
	r = glm::cross(-r, up); //-r.Cross(up);
	eye += r * x;
}


void Camera::Aim(float vertical_angle, float horizontal_angle) {		

	// kamera se bude otacet po jednotkove kouli
	angle_horiz += horizontal_angle;
	angle_vert += vertical_angle;

	float f_pi_2 = (float)M_PI / 2;

	// osetrit pretaceni pres hlavu a pod nohama
	// pridat korekci, protoze pokud je target shodny s up vektorem, ma LookAt nedefinovane chovani
	if (angle_vert >= f_pi_2)
		angle_vert = f_pi_2 - 0.0001f;
	if (angle_vert <= -f_pi_2)
		angle_vert = -f_pi_2 + 0.0001f;

	// uhly sferickych souradnic jsou velke a male fi
	float l_phi = f_pi_2 - angle_horiz;
	float u_phi = f_pi_2 - angle_vert;	

	target = glm::vec3(
		sin(u_phi) * cos(l_phi),
		cos(u_phi),
		sin(u_phi) * sin(l_phi)
	);
}


glm::mat4 Camera::GetMatrix() {
	/*
	if (isObserving) {		
		target = observeCenter;
		eye = Vector3f( float(sin(observeTimer.f_Time() * observeSpeed)), target.y, 
		float(cos(observeTimer.f_Time() * observeSpeed)) ) * observeRadius;

		target = observeCenter - eye;
		target.Normalize();
		//target.y = cos(observeTimer.f_Time());
		//target.z = 1.0f;

		//Move(1, 0, 0);
	}
	*/
	
	glm::mat4 m = glm::gtx::transform2::lookAt(eye, target + eye, up);
	return m;	
}



void Camera::DebugDump() {
	printf("----------------------------------------------------\n");
	printf("pos %f %f %f\n", eye.x, eye.y, eye.z);
	printf("target %f %f %f\n", target.x, target.y, target.z);
	printf("up  %f %f %f\n", up.x, up.y, up.z);
}

/*
void Camera::toggleObserve() {
	isObserving = !isObserving;

	if (isObserving)
		observeCenter = Vector3f(2.78f, 2.73f, 2.73f);
}
*/
