
#include "Physics.h"


Physics::Physics(void): m_car(NULL)
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
    m_overlappingPairCache = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));//new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_constraintSolver = new btSequentialImpulseConstraintSolver;
    
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_constraintSolver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

    m_debugDraw = new PhysicsDebugDraw;    
    m_dynamicsWorld->setDebugDrawer(m_debugDraw);

    m_checkpoint.Initialize(m_dynamicsWorld);
}

void Physics::Deinitialize()
{
    //remove the rigidbodies from the dynamics world and delete them
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
  //  for (int j = 0; j < m_collisionShapes.size(); j++)
  //      delete m_collisionShapes[j];

    if (m_car)
    {
        m_car->Deinitialize();
        delete m_car;
    }

    m_checkpoint.Deinitialize();

    delete m_dynamicsWorld;
    delete m_debugDraw;
    delete m_constraintSolver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Physics::StepSimulation(btScalar timeStep)
{
    if (m_car) m_car->Update(timeStep);

    int numSimSteps = m_dynamicsWorld->stepSimulation(timeStep, MAX_SIMULATION_SUBSTEPS, FIXED_SIMULATION_TIMESTEP);

    /*if (!numSimSteps)	std::cout << "Interpolated transforms" << std::endl;
	else  {
		if (numSimSteps > MAX_SIMULATION_SUBSTEPS) // detect dropping frames			
			std::cout << "Dropped " << numSimSteps - MAX_SIMULATION_SUBSTEPS << " simulation steps out of " << numSimSteps << std::endl;
        else
			std::cout << "Simulated " << numSimSteps << " steps" << std::endl;
	}*/
}

btRigidBody * Physics::AddRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape)
{
    btRigidBody* body = PhysicsUtils::CreateRigidBody(mass, startTransform, shape);
    
    m_dynamicsWorld->addRigidBody(body);

    return body;
}

btCollisionShape * Physics::CreateStaticCollisionShape(Mesh * mesh, const btVector3 & scale)
{
    int numTriangles = mesh->getFaces().size();
    int numVertices = mesh->getVertices().size();

    if (numTriangles < 1 || numVertices < 3)
        return NULL;

    btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray(numTriangles, (int*)(&mesh->getFaces()[0]), sizeof(glm::ivec3),
                                                                      numVertices, (btScalar*)(&mesh->getVertices()[0]), sizeof(glm::vec3));
        
    btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(tiva, true);        
    shape->setLocalScaling(scale);

    return shape;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(BaseModel * model, const btVector3 & scale)
{
    std::vector<btCollisionShape *> ret;
    
    for (unsigned int i = 0; i < model->getMeshes().size(); i++)
    {
        btCollisionShape *shape = CreateStaticCollisionShape(model->getMeshes()[i], scale);
        
        if (shape)
            ret.push_back(shape);
    }

    return ret;
}

std::vector<btCollisionShape *> Physics::CreateStaticCollisionShapes(BaseModel * model, float scale)
{
    return CreateStaticCollisionShapes(model, btVector3(scale, scale, scale));
}

void Physics::AddStaticModel(std::vector<btCollisionShape *> & collisionShapes, const btTransform & trans, bool debugDraw, const btVector3 & scale)
{
    for (unsigned int i = 0; i < collisionShapes.size(); i++)
    {
        btCollisionShape *colShape;
        
        if (scale != btVector3(1,1,1))
            colShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)collisionShapes[i], scale);
        else
            colShape = collisionShapes[i];

        btRigidBody *body = AddRigidBody(0, trans, colShape);

        if (!debugDraw)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
}
