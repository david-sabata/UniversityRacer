#ifndef PHYSICSCHECKPOINT_H
#define PHYSICSCHECKPOINT_H

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "GameTime.h"


#define CHECKPOINT_SCALE 0.1f

#define CHECKPOINT_SIZE_X 5.5f * CHECKPOINT_SCALE
#define CHECKPOINT_SIZE_Y 4.f * CHECKPOINT_SCALE
#define CHECKPOINT_SIZE_Z .5f * CHECKPOINT_SCALE

#define MAX_ROUNDS 1

class PhysicsCheckpoint
{
public:
    PhysicsCheckpoint(void): m_passedNum(-1), m_round(0) {};
    ~PhysicsCheckpoint(void) {};

    void Initialize(btDiscreteDynamicsWorld *refWorld)
    {
            m_ghostPairCallback = new btGhostPairCallback();
            refWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
         // refWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback); 

            m_ghostObject = new btGhostObject();
        
            btCollisionShape* checkpointShape = new btBoxShape(btVector3(CHECKPOINT_SIZE_X, CHECKPOINT_SIZE_Y, CHECKPOINT_SIZE_Z));
            m_ghostObject->setCollisionShape(checkpointShape);

            m_ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

            refWorld->addCollisionObject(m_ghostObject);
    }

    bool Collision(btRigidBody *body, const GameTime & gameTime)
    {
        if (PassedFinish())
            return false;
        
        for(int i = 0; i < m_ghostObject->getNumOverlappingObjects(); i++)
        {
            btRigidBody *rigidBody = static_cast<btRigidBody *>(m_ghostObject->getOverlappingObject(i));
            if (rigidBody == body)
            {
                if (m_passedNum+1 == m_transforms.size())
                {
                    m_round++;
                    if (PassedFinish())
                        m_finishTime = gameTime.Current();
                }
                
                m_passedNum = (m_passedNum+1) % m_transforms.size();                
                m_ghostObject->setWorldTransform(m_transforms[(m_passedNum+1) % m_transforms.size()]);

                if (PassedStart() && !PassedFinish())
                    m_startTime = gameTime.Current();

                return true;
            }
        }
        return false;
    }

    void Add(const btTransform & trans)
    { 
        if (m_transforms.size() == 0)
            m_ghostObject->setWorldTransform(trans);
        m_transforms.push_back(trans);
    }

    void Reset()
    {
        m_passedNum = -1;
        m_round = 0;
        m_ghostObject->setWorldTransform(m_transforms[0]);
    }

    bool PassedStart() { return m_passedNum == 0; }
    bool PassedFinish()  { return m_round == MAX_ROUNDS; }
    bool FirstPassed() { return m_passedNum != -1; }    
    int  PassedNum() { return m_passedNum; }    

    TimeSpan GetTime(const GameTime & gameTime)
    {
        if (PassedFinish())
            return m_finishTime - m_startTime;
        else if (FirstPassed())
            return gameTime.Current() - m_startTime;
        else
            return TimeSpan::Zero();
    }
        
    int Round() { return m_round; }
    int Count() { return m_transforms.size(); }

    btTransform & GetLastTrans() { return m_transforms[m_passedNum]; }


private:
    btGhostPairCallback *m_ghostPairCallback;
    btGhostObject *m_ghostObject;

    btAlignedObjectArray<btTransform> m_transforms;

    int m_passedNum;
    int m_round;

    TimeSpan m_startTime, m_finishTime;
    
    

};

#endif
