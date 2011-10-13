

#include "../bullet/src/btBulletDynamicsCommon.h"

int main(int argc, char *argv[])
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-10,0));
    ///create a few basic rigid bodies
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
    //keep track of the shapes, we release memory at exit.
    //make sure to re-use collision shapes among rigid bodies whenever possible!
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    collisionShapes.push_back(groundShape);
    
    
    return 0;
}