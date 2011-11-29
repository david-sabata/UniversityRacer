
#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "CarPhysics.h"
#include "PhysicsDebugDraw.h"

#include "BaseModel.h"


class Physics  // TODO Singleton
{
public:
    Physics(void); 
    ~Physics(void);

    void Initialize();
    void Deinitialize();

    void StepSimulation(btScalar timeStep);
    btRigidBody * AddRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape);

    void DebugDrawWorld();
    void AddCollisionShape(btCollisionShape * shape) { m_collisionShapes.push_back(shape); }
    CarPhysics * GetCar() { return m_car; }
    btDiscreteDynamicsWorld * GetDynamicsWorld() { return m_dynamicsWorld; }

    btCollisionShape* CreateTriMeshGround();

    void AddStaticModel(BaseModel *model, float scale);

private:

    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_constraintSolver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;

    btGhostPairCallback *m_ghostPairCallback;
    btGhostObject *m_ghostObject;
    int m_checkpointOffset;
    
    btDiscreteDynamicsWorld *m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    PhysicsDebugDraw *m_debugDraw;

    CarPhysics *m_car;
};

