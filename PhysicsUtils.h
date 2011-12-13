// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#ifndef PHYSICSUTILS_H
#define PHYSICSUTILS_H

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * Pomocne funkce pro praci s fyzikou, zejmena konverzni.
 */
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
