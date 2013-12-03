/*
objectPhysics.cpp
*/
#include "objectPhysics.h"

ObjectPhysics::ObjectPhysics() {
	active = false;
	world=NULL;
	objectShape=NULL;
    //objectMotionState=NULL;
    objectRigidBody=NULL;
    mass=0.0;
    fallInertia=btVector3(0.0,0.0,0.0);
    objectType = "None";
}

ObjectPhysics::~ObjectPhysics() {
	
}

void ObjectPhysics::cleanUp() {
	std::cout<<"cleaningUP"<<std::endl;
    if (objectRigidBody != NULL) {
    	removeFromWorld();
    	active = false;
    	//delete objectMotionState;
        delete objectRigidBody->getMotionState();
        delete objectRigidBody;
        delete objectShape;
    }
}

void ObjectPhysics::setCollisionShape(btCollisionShape* shape, glm::vec3 scaler) {
	if (objectShape==NULL) {
		objectShape=shape;
	    objectShape->setLocalScaling(btVector3(scaler.x, scaler.y, scaler.z));
	} else {
		//exists, but we're changing what it is:
		//remove from world
		//delete current
		//rebuild
		//add
	}
}

//void ObjectPhysics::setMotionState(glm::vec3 translate, glm::vec3 rotate) {
//	objectMotionState = new btDefaultMotionState(btTransform(btQuaternion(rotate.x, rotate.y, rotate.z,1),btVector3(translate.x,translate.y,translate.z)));
//}

void ObjectPhysics::setRigidBody(glm::vec3 translate, glm::vec3 rotate, std::string motionType) {
	//keep track of this
	objectType = motionType;
	if (objectRigidBody==NULL) {
		btDefaultMotionState* objectMotionState = new btDefaultMotionState(btTransform(btQuaternion(rotate.x, rotate.y, rotate.z,1),btVector3(translate.x,translate.y,translate.z)));
		//check that we can do this
		if(objectShape==NULL || objectMotionState==NULL) {
			std::cout<<"ERROR: Cannot set rigid body without CollisionShape and MotionState."<<std::endl;
			exit(EXIT_FAILURE);
		}
		//check if we've already done this
		if (motionType == "ground") {
	        //set mass and inertia
	        mass = 0;
	        fallInertia = btVector3(0,0,0);
	        //create construction info
	        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
	            objectMotionState,
	            objectShape,
	            fallInertia);
	        //create rigid body
	        //these can be the same, since we aren't moving the board at all...but maybe its safer if they aren't
	        objectRigidBody = new btRigidBody(rigidBodyCI);
	        objectRigidBody->setAnisotropicFriction(btVector3(0.04,0.04,0.04));
	        //just a quick check for moving and non-dynamic
	        //notStatic = true;
	    } else if (motionType == "static") {
	        //set mass and inertia
	        mass = 0;
	        fallInertia = btVector3(0,0,0);
	        //set construction info
	        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
	            objectMotionState,
	            objectShape,
	            fallInertia);
	        //create new rigid body
	        objectRigidBody = new btRigidBody(rigidBodyCI);
	        //enable kinetic objects
	        objectRigidBody->setRestitution(0.8);//between 0 and 1: 0 no bounce, 1 strong bounce
	        objectRigidBody->setCollisionFlags(objectRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	        objectRigidBody->setActivationState(DISABLE_DEACTIVATION);
	        //label as static
	        //notStatic = false;
	    } else { //dynamic
	        //set mass and inertia
	        mass = 1;
	        fallInertia = btVector3(0,0,0);
	        objectShape->calculateLocalInertia(mass, fallInertia);
	        //construct object
	        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, 
	            objectMotionState, 
	            objectShape, 
	            fallInertia);
	        //create rigid body
	        objectRigidBody = new btRigidBody(rigidBodyCI);
	        objectRigidBody->setRestitution(0.8);
	        objectRigidBody->setActivationState(DISABLE_DEACTIVATION);
	        //label as non static
	        //notStatic = true;
	    }
	} else {
		//rigidBody Exists, create new one

	}
}

void ObjectPhysics::setMotion(glm::vec3 scaler, glm::vec3 translate, glm::vec3 rotate, glm::vec3 inertia) {
	//set all values
	objectShape->setLocalScaling(btVector3(scaler.x, scaler.y, scaler.z));
	btDefaultMotionState* objectMotionState = new btDefaultMotionState(btTransform(btQuaternion(rotate.x, rotate.y, rotate.z,1),btVector3(translate.x,translate.y,translate.z)));
	fallInertia = btVector3(inertia.x,inertia.y,inertia.z);
	//now apply to object in world: remove, remake, add
	removeFromWorld();
	//set everything
	objectRigidBody->setMotionState(objectMotionState);
	objectRigidBody->setInvInertiaDiagLocal(fallInertia);
    objectRigidBody->setAngularVelocity(fallInertia);
    objectRigidBody->setLinearVelocity(fallInertia);
    addToWorld();
}

void ObjectPhysics::resetMotion(glm::mat4 model) {
	//get the new transform ready
    btTransform transform;
    transform.setFromOpenGLMatrix(glm::value_ptr(model));
    //remove the object (Whatever it is)
    removeFromWorld();
    //set the position of the rigid body
    objectRigidBody->clearForces();
    btVector3 tempInertia  = btVector3(0,0,0);
    //set inertia, velocity etc
    objectRigidBody->setAngularVelocity(tempInertia);
    objectRigidBody->setLinearVelocity(tempInertia);
    //the following two, sometimes work
    objectRigidBody->getMotionState()->setWorldTransform(transform);
    objectRigidBody->setWorldTransform(transform);
    objectRigidBody->setCcdMotionThreshold(1.0);
    /*if (objectType=="static") {
    	objectRigidBody->setCollisionFlags(objectRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    } else {
    	//objectRigidBody->setCollisionFlags(objectRigidBody->getCollisionFlags());
    }*/
    objectRigidBody->setActivationState(DISABLE_DEACTIVATION);
    objectRigidBody->activate(true);
    //add object back into both worlds
    addToWorld();
}

glm::mat4 ObjectPhysics::getCurrentTransformMat() {
	btTransform transform;
	objectRigidBody->getMotionState()->getWorldTransform(transform);
	btScalar * hmm = new btScalar[16];
    transform.getOpenGLMatrix(hmm);
    btVector3 translate = transform.getOrigin();
    /*std::cout<<"Is the Transform WORKING: "<<std::endl;
    std::cout<<translate[0]<<','<<translate[1]<<','<<translate[2]<<std::endl;
    for (int i=0; i<16; i++){
    	std::cout<<hmm[i]<<", ";
    }
    std::cout<<std::endl;*/
    glm::mat4 model = glm::make_mat4(hmm);
    return model;
}

glm::vec3 ObjectPhysics::getCurrentTranslateVec() {
	btVector3 trans;
	btTransform transform;
	objectRigidBody->getMotionState()->getWorldTransform(transform);
    trans = transform.getOrigin();
    return glm::vec3(trans[0],trans[1],trans[2]);
}

btTransform ObjectPhysics::getCurrentTransformbt() {
	btTransform transform;
	objectRigidBody->getMotionState()->getWorldTransform(transform);
    return transform;
}

btVector3 ObjectPhysics::getCurrentTranslatebt() {
	btVector3 trans;
	btTransform transform;
	objectRigidBody->getMotionState()->getWorldTransform(transform);
    trans = transform.getOrigin();
    return trans;
}

void ObjectPhysics::setMotion(btTransform transform) {
	removeFromWorld();
    
    //set the positions of both worlds to the new transform
    objectRigidBody->getMotionState()->setWorldTransform(transform);

    //add object back into both worlds
    addToWorld();
}

void ObjectPhysics::removeFromWorld() {
	if (active) {
		world->dynamicsWorld->removeRigidBody(objectRigidBody);
		active = false;
	} else {
		std::cout<<"Error: removing from world twice."<<std::endl;
	}
}

void ObjectPhysics::addToWorld() {
	if (!active) {
		world->dynamicsWorld->addRigidBody(objectRigidBody);
		active = true;
	}else {
		std::cout<<"Error: adding to world twice."<<std::endl;
	}
}

void ObjectPhysics::setCCD(int dist) {
	objectRigidBody->setCcdMotionThreshold(dist);
}