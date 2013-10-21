/*
Physics World: Store stuff about our physics setup here, so that it doesn't clutter main so badly
*/
#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btAlignedObjectArray.h>
//#include <btBulletWorldImporter.h>

class PhysicsWorld{
public:
	PhysicsWorld();
	~PhysicsWorld();
	btDiscreteDynamicsWorld* dynamicsWorld;
	void stepWorld(float someTime, int whatisthis);
	void addRigidBodyObject(btRigidBody* object);
private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConf;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

};