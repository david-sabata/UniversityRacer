
#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <cmath>
#include "Utils.h"

class CarPhysics
{
public:    

    struct CarConfig
    {
        CarConfig(): mass(800.f),
                     maxEngineForce(10000.f),  //this should be engine/velocity dependent
                     maxBreakingForce(100.f),
                     steeringIncrement(0.04f),
                     steeringClamp(0.75f),
                     
                     wheelFriction(1000.f),  //BT_LARGE_FLOAT;
                     wheelRadius(0.5f),
                     wheelWidth(0.4f),
                     connectionHeight(1.0f),  // 1.2
                     connectionWidth(0.725f), 
                     connectionLength(1.4f), // 1.5
                     wheelDirectionCS0(0.f, -1.f, 0.f),
                     wheelAxleCS(-1.f, 0.f, 0.f),
                     
                     suspensionStiffness(10.f),  //20
                     suspensionDamping(2.3f),  //2.3
                     suspensionCompression(4.4f),
                     suspensionRestLength(0.6f),
                     rollInfluence(0.1f)   //1.0f;
                     {}

        btScalar mass;
        btScalar maxEngineForce, maxBreakingForce, steeringIncrement, steeringClamp;
        btScalar wheelFriction, wheelRadius, wheelWidth, connectionHeight, connectionWidth, connectionLength;
        btVector3 wheelDirectionCS0, wheelAxleCS;
        btScalar suspensionStiffness, suspensionDamping, suspensionCompression, suspensionRestLength, rollInfluence;
       // int maxProxies 32766, maxOverlap 65535;
    };

    CarPhysics(void);
    ~CarPhysics(void);

    btCollisionShape* CreateVehicleUpperShape();
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
    glm::mat4 GetWorldTransform()
    {
        btTransform trans = m_vehicle->getChassisWorldTransform();
        btScalar m[16];
        trans.getOpenGLMatrix(m);    
        
        return mat4_from_arr(m);    
    }

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

