#ifndef PHYSICSUTILS_H
#define PHYSICSUTILS_H

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

class PhysicsUtils
{
public:
    PhysicsUtils(void);
    ~PhysicsUtils(void);

    static btRigidBody * PhysicsUtils::CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape);

    static btTransform btTransFrom(const btVector3 & origin, const btQuaternion & rot = btQuaternion(0,0,0));
    static btTransform btTransFrom(const glm::mat4 & m);
    static glm::mat4 glmMat4From(const btTransform & trans);
};

#endif
