#ifndef PHYSICS_H
#define PHYSICS_H

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsUtils.h"
#include "PhysicsCar.h"
#include "PhysicsCheckpoint.h"
#include "PhysicsDebugDraw.h"
#include "BaseModel.h"

#define MAX_SIMULATION_SUBSTEPS   10
#define FIXED_SIMULATION_TIMESTEP 1/60.f

class Physics
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

    static btCollisionShape * CreateStaticCollisionShape(Mesh * mesh, const btVector3 & scale = btVector3(1,1,1));
    static std::vector<btCollisionShape *> CreateStaticCollisionShapes(BaseModel * model, const btVector3 & scale);
    static std::vector<btCollisionShape *> CreateStaticCollisionShapes(BaseModel * model, float scale = 1.f);

    void AddStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw = true, const btVector3 & scale = btVector3(1,1,1));
    
    PhysicsCar * GetCar() { return m_car; }
    PhysicsCheckpoint & Checkpoint() { return m_checkpoint; }

    PhysicsDebugDraw *GetDebugDrawer() { return m_debugDraw; }
    btDiscreteDynamicsWorld * GetDynamicsWorld() { return m_dynamicsWorld; }    

    void AddCar(const btTransform & trans) { (m_car = new PhysicsCar())->Initialize(m_dynamicsWorld, trans); }
    

private:

    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_constraintSolver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    
    btDiscreteDynamicsWorld *m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    PhysicsCar *m_car;
    PhysicsCheckpoint m_checkpoint;
    PhysicsDebugDraw *m_debugDraw;
};

#endif
