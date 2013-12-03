/*
Might be easier to have physics for the meshes out here too...
*/

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "physics.h"

class ObjectPhysics {
public:
	std::string objectType;

	PhysicsWorld* world;
    btCollisionShape* objectShape;// = NULL;
    //btDefaultMotionState* objectMotionState;// = NULL;
    btRigidBody* objectRigidBody;// = NULL;
    btScalar mass;// = 1;
    btVector3 fallInertia;//(0,0,0);
    bool active;
    glm::vec3 originalTranslate;
    glm::vec3 originalRotate;

    ObjectPhysics();
    ~ObjectPhysics();
    void cleanUp();
    void setCollisionShape(btCollisionShape* shape, glm::vec3 scaler);//std::string shape, glm::vec3 scaler, int totalVerts, int totalTriangles);
    //void setMotionState(glm::vec3 translate, glm::vec3 rotate);
    void setRigidBody(glm::vec3 translate, glm::vec3 rotate, std::string motionType);
    void setMotion(glm::vec3 scaler, glm::vec3 translate, glm::vec3 rotate, glm::vec3 inertia);
    void resetMotion(glm::mat4 model);
    glm::mat4 getCurrentTransformMat();
    glm::vec3 getCurrentTranslateVec();
    btTransform getCurrentTransformbt();
    btVector3 getCurrentTranslatebt();
    void setMotion(btTransform transform);
    void removeFromWorld();
    void addToWorld();
    void setCCD(int dist=1);
};