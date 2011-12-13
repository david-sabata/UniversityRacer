// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#include "PhysicsUtils.h"


PhysicsUtils::PhysicsUtils(void)
{
}


PhysicsUtils::~PhysicsUtils(void)
{
}

btRigidBody * PhysicsUtils::CreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape)
{
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f)  //rigidbody is dynamic if and only if mass is non zero, otherwise static
        shape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

    btRigidBody* body = new btRigidBody(cInfo);
    body->setContactProcessingThreshold(0);

    return body;
}

btTransform PhysicsUtils::btTransFrom(const btVector3 & origin, const btQuaternion & rot)
{
    btTransform ret;
    ret.setIdentity();
    ret.setRotation(rot);
    ret.setOrigin(origin);

    return ret;
}

btTransform PhysicsUtils::btTransFrom(const glm::mat4 & m)
{
    btMatrix3x3 mat(m[0].x, m[1].x, m[2].x,
                    m[0].y, m[1].y, m[2].y,
                    m[0].z, m[1].z, m[2].z);
    btVector3 vec(m[3].x, m[3].y, m[3].z);
    
    return btTransform(mat, vec);
}

glm::mat4 PhysicsUtils::glmMat4From(const btTransform & trans)
{
    btMatrix3x3 m = trans.getBasis();
    btVector3 v = trans.getOrigin();

    return glm::mat4( m[0].x(), m[1].x(), m[2].x(), 0,
                      m[0].y(), m[1].y(), m[2].y(), 0,
                      m[0].z(), m[1].z(), m[2].z(), 0,
                         v.x(),    v.y(),    v.z(), 1);
}
