#ifndef CARPHYSICS_H
#define CARPHYSICS_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include "CarConfig.h"
#include "PhysicsUtils.h"

class CarPhysics
{
public:    

    CarPhysics(void);
    ~CarPhysics(void);

    btCollisionShape* CreateVehicleShape();

    void Initialize(btDiscreteDynamicsWorld *refWorld);
    void Deinitialize();

    void SetLastCheckpointPos(btVector3 & pos) { m_lastCheckpointPos = pos; }

    void Reset();
    void Update(btScalar timeStep);

    void TurnLeft();    
    void TurnRight();    
    void Forward();    
    void Backward();    
    void HandBrake();

    btRaycastVehicle* GetVehicle() { return m_vehicle; }
    btTransform GetWorldTransform();

private:
    btDiscreteDynamicsWorld *m_refDynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    btVehicleRaycaster* m_vehicleRayCaster;
    btRaycastVehicle* m_vehicle;
    btRigidBody* m_carChassis;

    CarConfig m_cfg;
    bool turned;
    btVector3 m_lastCheckpointPos;

    float m_engineForce;
    float m_breakingForce;
    float m_vehicleSteering;
};

#endif
