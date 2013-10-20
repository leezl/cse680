/*
Physics World: Store stuff about our physics setup here, so that it doesn't clutter main so badly
*/
#include <btBulletDynamicCommon.h>

class PhysicsWorld{
public:
	PhysicsWorld();
	~PhysicsWorld();
	btDiscreteDynamicsWorld* dynamicsWorld;
	void stepWorld(float someTime, int whatisthis);
	void addRigidBodyObject(btRigidBody* object);
private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConf* collisionConf;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseCOnstraintSolver* solver;

};