
#include "PhysicsCheckpoint.h"


 void PhysicsCheckpoint::Initialize(btDiscreteDynamicsWorld *refWorld)
{
        m_ghostPairCallback = new btGhostPairCallback();
        refWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
     // refWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback); 

        m_ghostObject = new btGhostObject();
    
        m_checkpointShape = new btBoxShape(btVector3(CHECKPOINT_SIZE_X, CHECKPOINT_SIZE_Y, CHECKPOINT_SIZE_Z));
        m_ghostObject->setCollisionShape(m_checkpointShape);

        m_ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        refWorld->addCollisionObject(m_ghostObject);
}

void PhysicsCheckpoint::Deinitialize()
{
    delete m_ghostObject;
    delete m_checkpointShape;
    delete m_ghostPairCallback;
}

bool PhysicsCheckpoint::Collision(btRigidBody *body, const GameTime & gameTime)
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

void PhysicsCheckpoint::Add(const btTransform & trans)
{ 
    if (m_transforms.size() == 0)
        m_ghostObject->setWorldTransform(trans);
    m_transforms.push_back(trans);
}

void PhysicsCheckpoint::Reset()
{
    m_passedNum = -1;
    m_round = 0;
    m_ghostObject->setWorldTransform(m_transforms[0]);
}

TimeSpan PhysicsCheckpoint::GetTime(const GameTime & gameTime)
{
    if (PassedFinish())
        return m_finishTime - m_startTime;
    else if (FirstPassed())
        return gameTime.Current() - m_startTime;
    else
        return TimeSpan::Zero();
}