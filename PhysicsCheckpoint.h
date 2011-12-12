#ifndef PHYSICSCHECKPOINT_H
#define PHYSICSCHECKPOINT_H

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "GameTime.h"

/// meritko kolizniho telesa checkpointu 
#define CHECKPOINT_SCALE 0.1f

/// rozmery kolizniho telesa checkpointu 
#define CHECKPOINT_SIZE_X 5.5f * CHECKPOINT_SCALE
#define CHECKPOINT_SIZE_Y 4.f * CHECKPOINT_SCALE
#define CHECKPOINT_SIZE_Z .5f * CHECKPOINT_SCALE

/// pocet kol
#define MAX_ROUNDS 1

/**
 * Sprava checkpointu s detekci kolizi a pocitanim casu.
 * Prace s btGhostObject na zaklade http://bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Callbacks_and_Triggers#btGhostObject
 */
class PhysicsCheckpoint
{
public:
    PhysicsCheckpoint(void): m_passedNum(-1), m_round(0) {}
    ~PhysicsCheckpoint(void) {}

    void Initialize(btDiscreteDynamicsWorld *refWorld);
    void Deinitialize();

    bool Collision(btRigidBody *body, const GameTime & gameTime);
    void Add(const btTransform & trans);
    void Reset();

    bool PassedStart() { return m_passedNum == 0; }
    bool PassedFinish()  { return m_round == MAX_ROUNDS; }
    bool FirstPassed() { return m_passedNum != -1; }    
    int  PassedNum() { return m_passedNum; }    

    int Round() { return m_round; }
    int Count() { return m_transforms.size(); }

    btTransform & GetLastTrans() { return m_transforms[m_passedNum]; }
    TimeSpan GetTime(const GameTime & gameTime);

private:
    btGhostPairCallback *m_ghostPairCallback;
    btGhostObject *m_ghostObject;
    btCollisionShape* m_checkpointShape;

    btAlignedObjectArray<btTransform> m_transforms;

    int m_passedNum, m_round;
    
    TimeSpan m_startTime, m_finishTime;
};

#endif
