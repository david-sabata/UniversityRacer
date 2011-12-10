#ifndef PHYSICSCAR_H
#define PHYSICSCAR_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include "PhysicsCarConfig.h"
#include "PhysicsUtils.h"

class PhysicsCar
{
public: 
    enum WHEELID {  // z pohledu ridice
        WHEEL_FRONTLEFT = 0,
        WHEEL_FRONTRIGHT,
        WHEEL_REARLEFT,
        WHEEL_REARRIGHT,
        WHEEL_COUNT  // zarazka
    };

    PhysicsCar(void);
    ~PhysicsCar(void);

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
