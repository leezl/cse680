/*
Storage for objects: buffer id, model matrix

Delete other load Funcitons when done picking one
*/

#include "mesh.h"

#ifndef OBJECT_H_
#define OBJECT_H_

/*
Need: separate draw for each material: different lighting, so: need to keep faces split by material
Keep in Assimp aiScene: store the aiScene, and iterate through it in the draw function.
*/

class Object {
  private:
    std::vector<GLuint> elementBuffers, geometryBuffers, normalBuffers, colorBuffers, textureBuffers;
    //we will switch between these meshes: ie each one is a separate object, but this object could be any of them at a given time
    std::vector< Mesh > meshes;
    unsigned int meshChoice;
    GLenum ErrorCheckValue;
    glm::vec3 oldTranslate;
    glm::vec3 oldRotate;
    glm::vec3 oldPostRotate;
    glm::vec3 oldScale;
    glm::vec3 originalTranslate;
    glm::vec3 originalRotate;
    glm::vec3 originalPostRotate;
    glm::vec3 originalScale;
    glm::mat4 originalModel;
    std::string motionType;
    //All of a sudden it makes more sense to have: 
    //a queue of transforms: rotate, translate, rotate...
    //SO we cna tell it what orde rto make them in...

    void setDefaultInit();
    void checkError(std::string where);
    glm::vec4 worldTrans(glm::vec4 vector, glm::mat4 transform);
    void updateStorage();
    bool checkCollisions();
    void removeNextMove();
    void resetPhysics();
    glm::vec4 getNextMax();
    glm::vec4 getNextMin();
    glm::vec4 getNextCenter();
  public:
    std::string name;//name given by main
    unsigned int maxHistory;
    std::queue< glm::mat4 > oldModels;
    std::vector< collisionObject > colliders;
    glm::mat4 model;//this objects model
    glm::vec3 nextTranslate;
    glm::vec3 nextRotate;
    glm::vec3 nextPostRotate;
    glm::vec3 nextScale;
    bool collidedWith;//determine which shader to use
    bool frozen;
    bool reset;
    Object(std::string namer = "defaultName");
    Object(glm::vec3 max, glm::vec3 min, std::string newName="defaultName");
    Object(std::string path, std::string filename);
    ~Object();
    glm::mat4 getModel();
    glm::mat4 getModelNoScale();
    void initializeObject();
    void addPostRotation(glm::vec3 postRotate);
    void setCollider(Object* what, bool in=true, float margin=0.0, bool edge=true, bool reset=false, bool freeze=false);
    void setTransforms(glm::vec3 trans, glm::vec3 rot, glm::vec3 sca, PhysicsWorld* world, std::string moves = "static", std::string shape="convex");
    void cleanUp();
    void drawObject();
    void flipNormals();
    void setBrightness(glm::vec4 bright);
    void setPrograms(std::vector< Program* > program);
    void setProgram(Program* program);
    void setPhysics(std::string shapeType="mesh", std::string motionType="ground", PhysicsWorld* world=NULL);
    void updateModel();
    void resetPosition(glm::vec3 trans, glm::vec3 rot, glm::vec3 scal);
    void resetPosition();
    void addMesh(std::string path, std::string filename);
    void swapMesh(unsigned int newMesh);
    void enablePhysics();
    void disablePhysics();
    BoardConfig getBoardStop();
    //following returns max, min center transformed by the model matrix
    glm::vec4 getCurrentMax();
    glm::vec4 getCurrentMin();
    glm::vec4 getCurrentCenter();
    //following return max, min, center original
    glm::vec4 getGlobalMax();
    glm::vec4 getGlobalMin();
    glm::vec4 getGlobalCenter();
};

#endif  // OBJECT_H_
