

#include "Physics.h"


Physics::Physics(void):m_checkpointOffset(0)
{
    Initialize();
}

Physics::~Physics(void)
{
    Deinitialize();
}

void Physics::Initialize()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    m_overlappingPairCache = new btAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));//new btDbvtBroadphase();
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_constraintSolver = new btSequentialImpulseConstraintSolver;
    
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_constraintSolver, m_collisionConfiguration);
    m_ghostPairCallback = new btGhostPairCallback();
    m_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
   // m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback()); 
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

    m_debugDraw = new PhysicsDebugDraw;    
    m_dynamicsWorld->setDebugDrawer(m_debugDraw);
    // dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    m_car = new CarPhysics();
    m_car->Initialize(m_dynamicsWorld);


    btCollisionShape* checkpointShape = new btBoxShape(btVector3(2.f, 2.f, 0.1f));
    this->AddCollisionShape(checkpointShape);
    
    m_ghostObject = new btGhostObject();
    m_ghostObject->setCollisionShape(checkpointShape);
    m_ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    m_dynamicsWorld->addCollisionObject(m_ghostObject);//, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, -3, -5));  //-10
    m_ghostObject->setWorldTransform(transform);
}

void Physics::DebugDrawWorld()
{
    m_dynamicsWorld->debugDrawWorld();
    
 /*   btCompoundShape* compoundShape = static_cast<btCompoundShape*>(m_car->GetVehicle()->getRigidBody()->getCollisionShape());
    
    for (int i = 0; i < compoundShape->getNumChildShapes(); i++)
    {
        btTransform childTrans = compoundShape->getChildTransform(i);
        btCollisionShape* colShape = compoundShape->getChildShape(i);
                 
        btTransform trans = m_car->GetVehicle()->getRigidBody()->getWorldTransform() * childTrans;
        btScalar m[16];
        trans.getOpenGLMatrix(m);
    
        m_debugDraw->DrawConvexShape(m, colShape);
    }*/
}

void Physics::Deinitialize()  //cleanup in the reverse order of creation/initialization
{
    //remove the rigidbodies from the dynamics world and delete them
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        m_dynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++)
        delete m_collisionShapes[j];
    
    //delete m_indexVertexArrays;
    //delete m_vertices;

    //delete dynamics world
    delete m_dynamicsWorld;
            
    delete m_car;
    //delete m_vehicleRayCaster;
    //delete m_vehicle;
    //delete m_wheelShape;

    delete m_constraintSolver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Physics::StepSimulation(btScalar timeStep)
{
    m_car->Update(timeStep);
    m_dynamicsWorld->stepSimulation(timeStep, 10);

    for(int i = 0; i < m_ghostObject->getNumOverlappingObjects(); i++)
    {
        btRigidBody *pRigidBody = static_cast<btRigidBody *>(m_ghostObject->getOverlappingObject(i));
        if (pRigidBody == m_car->GetVehicle()->getRigidBody())
        {
            btTransform transform;
            transform.setIdentity();
            m_car->SetLastCheckpointPos(btVector3(0.f, -3.f, -5.f - m_checkpointOffset));
            m_checkpointOffset += 10;
            transform.setOrigin(btVector3(0.f, -3.f, -5.f - m_checkpointOffset));  //-10
            m_ghostObject->setWorldTransform(transform);            
            
            std::cout << "collision";// << std::endl;

        }
        // do whatever you want to do with these pairs of colliding objects
    }
}

btRigidBody * Physics::AddRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape)
{
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f)  //rigidbody is dynamic if and only if mass is non zero, otherwise static
        shape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

    btRigidBody* body = new btRigidBody(cInfo);
    //body->setContactProcessingThreshold(BT_LARGE_FLOAT);

    m_dynamicsWorld->addRigidBody(body);

    return body;
}

btCollisionShape* Physics::CreateTriMeshGround()
{
    int vertStride = sizeof(btVector3);
    int indexStride = 3*sizeof(int);

    const float TRIANGLE_SIZE=20.f;
    const int NUM_VERTS_X = 20;
    const int NUM_VERTS_Y = 20;
    const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;    
    
    const int totalTriangles = 2*(NUM_VERTS_X-1)*(NUM_VERTS_Y-1);

    btTriangleIndexVertexArray*    m_indexVertexArrays;
    btVector3*    m_vertices;

    m_vertices = new btVector3[totalVerts];
    int* gIndices = new int[totalTriangles*3];    

    for (int i = 0; i < NUM_VERTS_X; i++)
    {
        for (int j = 0; j < NUM_VERTS_Y; j++)
        {
            float wl = .2f;
            float height = 0;//0.f;//20.f*sinf(float(i)*wl)*cosf(float(j)*wl);  //0.f;//
            if (j == 8)
                height = static_cast<float>(-i);
            m_vertices[i+j*NUM_VERTS_X].setValue((i-NUM_VERTS_X*0.5f)*TRIANGLE_SIZE, height, (j-NUM_VERTS_Y*0.5f)*TRIANGLE_SIZE);
        }
    }

    int index=0;
    for (int i = 0; i < NUM_VERTS_X - 1; i++)
    {
        for (int j=0;j<NUM_VERTS_Y-1;j++)
        {
            gIndices[index++] = j*NUM_VERTS_X+i;
            gIndices[index++] = j*NUM_VERTS_X+i+1;
            gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;

            gIndices[index++] = j*NUM_VERTS_X+i;
            gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;
            gIndices[index++] = (j+1)*NUM_VERTS_X+i;
        }
    }
    
    m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles, gIndices, indexStride, totalVerts, (btScalar*) &m_vertices[0].x(), vertStride);


    btBvhTriangleMeshShape *ret = new btBvhTriangleMeshShape(m_indexVertexArrays, true);
   // ret->setLocalScaling(btVector3(0.1, 0.1, 0.1));

    return ret; 
}


void Physics::AddStaticModel(BaseModel * model)
{
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(0, 0, 0));

    
    for (unsigned int i = 0; i < model->getMeshes().size(); i++)
    {
        Mesh *m = model->getMeshes()[i];

        if (m->getVertices().size() == 0)
            continue;

        if (m->getFaces().size() == 0)
            continue;

       /* int* gIndices = new int[m->getFaces().size()*3];

        for (unsigned int j = 0; j < m->getFaces().size(); j++)
        {
            gIndices[j] = static_cast<int>(m->getFaces()[j].x);
            gIndices[j+1] = static_cast<int>(m->getFaces()[j].y);
            gIndices[j+2] = static_cast<int>(m->getFaces()[j].z);
        }*/

     /*   int numTriangles = m->getFaces().size();
        int *triangleIndexBase = (int*)&m->getFaces()[0].x;
        int triangleIndexStride = sizeof(glm::vec3);
        int numVertices = m->getVertices().size();
        btScalar *vertexBase = (btScalar*)&m->getVertices()[0].x;
        int vertexStride = sizeof(glm::vec3);

        btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numTriangles, triangleIndexBase, triangleIndexStride, numVertices, vertexBase, vertexStride);
        btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);
        
        AddRigidBody(0, tr, shape);*/
    }
   
 //  m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles, gIndices, indexStride, totalVerts, (btScalar*) &m_vertices[0].x(), vertStride);

}
