#ifndef PHYSICS_H
#define PHYSICS_H

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsUtils.h"
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

    void DebugDrawWorld() { m_dynamicsWorld->debugDrawWorld(); } 
    void AddCollisionShape(btCollisionShape * shape) { m_collisionShapes.push_back(shape); }


    //void AddStaticModel(BaseModel * model, const btTransform & trans, float scale = 1.f, bool debugDraw = true);

    static std::vector<btCollisionShape *> CreateStaticCollisionShapes(BaseModel * model, float scale = 1.f);
    void AddStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true);
    
    CarPhysics * GetCar() { return m_car; }
    btDiscreteDynamicsWorld * GetDynamicsWorld() { return m_dynamicsWorld; }    
    PhysicsDebugDraw *GetDebugDrawer() { return m_debugDraw; }

    void AddCar(const btTransform & trans)
    {
        m_car = new CarPhysics();
        m_car->Initialize(m_dynamicsWorld, trans);
    }



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

#endif
