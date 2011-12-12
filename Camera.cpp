#include "Camera.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;


Camera::Camera(void): currentSettleTime(0.f) {
	Reset();
}


void Camera::Reset() {
    /*eye = glm::vec3(0.0f, 0.0f, -10.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);		
	target = glm::vec3(0.0f, 0.0f, 1.0f);*/
    
    eye = glm::vec3(38.435f, 11.41f, -27.451f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);		
	target = glm::vec3(-0.623f, -0.124f, 0.772f);
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


/**
 * Kod naportovan z True Axis Physics SDK (http://trueaxis.com/physics/index.html)
 * Konkretne ze souboru DemoCars/DemoCars.cpp z archivu http://trueaxis.com/physics/downloadDemosSource.html
 * A to konkretne z funkce "void UpdateCamera(float fDt)"
 * Pridana nezavislost pohybu kamery na FPS a upravy hodnot konstant aby sedeli pro nase meritko.
 */
void Camera::Follow(glm::mat4 & targetMat, glm::vec3 targetVelocity, const GameTime & gameTime)
{
    const glm::vec3 lookAtPos(0, 1, 0);
    const glm::vec3 lookFromPos(0, 1, 1);  // 0, 3, -2
    float viewDistance = 2.5f; // 10.f
    const float settleTime = 2.f;

    glm::mat3 targetRot(targetMat);
    glm::vec3 targetPos(targetMat[3]);
    
    // Using statics here because of lazy programming
    static glm::vec3 lastBaseLookFromPos = targetPos - targetRot[2] * 20.f;
    static float facing = 1.f;
  //static float currentSettleTime = 0.f;

    // camera intro
    if (currentSettleTime < settleTime)
    {
        glm::vec3 defaultPos = targetPos - targetRot[2] * 20.f;
        if (currentSettleTime > settleTime - 1.f)
            lastBaseLookFromPos += (settleTime - currentSettleTime) * (defaultPos - lastBaseLookFromPos);
        else
            lastBaseLookFromPos = defaultPos;
        viewDistance += (settleTime - currentSettleTime) * (settleTime - currentSettleTime);
        currentSettleTime += gameTime.Elapsed().ms() * 0.001f;
    }

    // First we nudge the previous camera position to get it to behave exactly the way we want.
    // If the object is moving towards the camera then nudge the camera
    // side was a little so it goes around.
    // This avoids camera going over the top of the car and flipping when 
    // changing velocity when going into reverse while looking forwards.
    float nudge = glm::length(lastBaseLookFromPos - targetPos);
    if (nudge < viewDistance)
    {
        nudge = viewDistance - nudge;
        lastBaseLookFromPos -= targetRot[0] * nudge * 1.1f;
        lastBaseLookFromPos.y += nudge * 0.2f;
    }

    // Nudged the camera a bit so that it swings around a bit more to face
    // the direction that the car is moving.
    // This makes it a little easier to see around corners.
    lastBaseLookFromPos -= targetVelocity * (gameTime.Elapsed().ms() * 0.000001f);

    // Nudge the camera so that is a bit more likely to look in the direction
    // that the car is facing. This helps most when maneuvering at low speed.
    glm::vec3 facingNudge = targetRot[2] * 0.2f * facing;
    facingNudge.y = 0.f;
    lastBaseLookFromPos -= facingNudge;

    // Smoothly choose whether the car is facing forwards or backwards for the above code.
    float dot = glm::dot(targetPos - lastBaseLookFromPos, targetRot[2]);
    facing = dot * 0.1f;
    if      (facing >  1.f) facing =  1.f;  // ClampPosNeg
    else if (facing < -1.f) facing = -1.f;

    // Now we make the camera follow the new object position.
    glm::vec3 lookDirection = targetPos - lastBaseLookFromPos;
    float mag = glm::length(lookDirection);
    if (mag > 0.f)
        lookDirection /= mag;
    else
        lookDirection = glm::vec3(0,0,1);   // It would be much better to use the last valid value

    glm::vec3 currentLookFromPos = targetPos - lookDirection * viewDistance;
    
    lastBaseLookFromPos = currentLookFromPos;

    if (glm::abs(lookDirection.y) <= 0.99f)
    {
        glm::mat4 tmp = glm::lookAt(glm::vec3(0), lookDirection, glm::vec3(0, 1, 0));
        glm::mat3 m(tmp);

        currentLookFromPos += lookFromPos * m;
        glm::vec3 currentLookAtPos = lookDirection;
     // glm::vec3 currentLookAtPos = targetPos + lookAtPos * m;

        eye = currentLookFromPos;
        target = currentLookAtPos;
        up = glm::vec3(0, 1, 0);
    }
}
