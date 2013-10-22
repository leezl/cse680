/*
Physics World cpp
*/
#include "physics.h"

PhysicsWorld::PhysicsWorld() {
	std::cout<<"Initializing World Dynamics "<<std::endl;
	broadphase = new btDbvtBroadphase();
	collisionConf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConf);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConf);
	dynamicsWorld->setGravity(btVector3(0,-10,0));
}

//either call this or access dynamicsWorld directly: I made it public
void PhysicsWorld::addRigidBodyObject(btRigidBody* object) {
	if (object != NULL) {
		dynamicsWorld->addRigidBody(object);
		std::cout<<"ADDED RIGID BODY"<<std::endl;
	} else {
		std::cerr<<"Why was that rigidBody pointer NULL?"<<std::endl;
		exit(EXIT_FAILURE);
	}
}

void PhysicsWorld::stepWorld(float someTime, int whatisthis) {
	dynamicsWorld->stepSimulation(someTime, whatisthis);
}

PhysicsWorld::~PhysicsWorld() {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConf;
	delete broadphase;
}