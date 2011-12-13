// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#ifndef PHYSICSCAR_H
#define PHYSICSCAR_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include "PhysicsCarConfig.h"
#include "PhysicsUtils.h"

/**
 * Fyzikalni simulace auta.
 * Cerpano z dokumentu Bullet 2.79 Physics SDK Manual a Vehicle Simulation With Bullet (http://tinyurl.com/ydfb7lm).
 * Dale cerpano z VehicleDemo, z nehoz byla vyuzita zakladni kostra kodu.
 * VehicleDemo a Bullet 2.79 Physics SDK Manual jsou obsazeny v archivu http://code.google.com/p/bullet/downloads/detail?name=bullet-2.79-rev2440.zip
 */
class PhysicsCar
{
public: 
    /**
     * Identifikatory kol auta; z pohledu ridice.
     */
    enum WHEELID {
        WHEEL_FRONTLEFT = 0,
        WHEEL_FRONTRIGHT,
        WHEEL_REARLEFT,
        WHEEL_REARRIGHT,
        WHEEL_COUNT  // zarazka
    };

    PhysicsCar(void): m_engineForce(0.f), m_breakingForce(0.f), m_vehicleSteering(0.f), m_turned(false) {}
    ~PhysicsCar(void) {}

    btCollisionShape* CreateVehicleShape();

    void Initialize(btDiscreteDynamicsWorld *refWorld, const btTransform & trans);
    void Deinitialize();

    void Reset(const btTransform & trans);
    void Update(btScalar timeStep = 1/60.f);

    void TurnLeft();    
    void TurnRight();    
    void Forward();    
    void Backward();    
    void Brake();

    btRaycastVehicle* GetVehicle() { return m_vehicle; }
    btTransform GetWorldTransform();
    btTransform GetInitialTransform() { return m_initialTrans; }

private:
    btDiscreteDynamicsWorld *m_refDynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    btVehicleRaycaster* m_vehicleRayCaster;
    btRaycastVehicle* m_vehicle;
    btRigidBody* m_carChassis;

    btTransform m_initialTrans;

    CarConfig m_cfg;
    
    float m_engineForce;
    float m_breakingForce;
    float m_vehicleSteering;
    bool m_turned;
};

#endif
