/*
Physics World cpp
*/

PhysicsWorld::PhysicsWorld() {
	broadphase = new btDbvtBroadphase();
	collisionConf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConf);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));
}

PhysicsWorld::addRigidBodyObject(btRigidBody* object) {
	if (object != NULL) {
		dynamicsWorld->addRigidBody(object);
	} else {
		std::cerr<<"WHy was that rigidBody pointer NULL?"<<std::endl;
	}
}

PhysicsWorld::~PhysicsWorld() {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConf;
	delete broadphase;
}