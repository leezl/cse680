/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/
#include "object.h"

Object::Object(std::string namer) {
    name = namer;
    setDefaultInit();
    //add default cube mesh
    meshes.push_back(Mesh(namer));
}

Object::Object(glm::vec3 maxer, glm::vec3 miner, std::string newName) {
    //create undrawable object to act as a "space"
    //used for collisions 
    name = newName;
    setDefaultInit();

    meshes.push_back(Mesh(maxer, miner, newName));
}

Object::Object(std::string path, std::string filename){
    name = filename;
    setDefaultInit();
    //load a mesh form a file
    meshes.push_back(Mesh(path, filename));
}

void Object::setDefaultInit() {
    motionType = "NONE";
    //default to only mesh
    meshChoice = 0;
    collidedWith = false;
    frozen = false;
    reset = false;
    nextTranslate = oldTranslate = originalTranslate = glm::vec3(0.0,0.0,0.0);
    nextRotate = oldRotate = originalRotate = glm::vec3(0.0,0.0,0.0);
    nextPostRotate = oldPostRotate = originalPostRotate = glm::vec3(0.0,0.0,0.0);
    nextScale = oldScale = originalScale = glm::vec3(1.0,1.0,1.0);
    //set model matrix
    model = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    model = glm::rotate(model, nextPostRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextPostRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextPostRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
    originalModel = model;
    maxHistory=2;
}

Object::~Object(){
    //had cleanUp here, but may not have initialized so...
}

BoardConfig Object::getBoardStop() {
    return meshes[meshChoice].board;
}

void Object::cleanUp(){
    for (unsigned int i =0; i<meshes.size(); i++) {
        meshes[i].cleanUp();
    }
}

void Object::setPrograms(std::vector< Program* > programs) {
    for (unsigned int i=0; i<meshes.size(); i++) {
        meshes[i].setProgram(programs);
    }
}

void Object::setProgram(Program* program) {
    for (unsigned int i=0; i<meshes.size(); i++) {
        meshes[i].setProgram(program);
    }
}

void Object::addPostRotation(glm::vec3 postRotate){
    nextPostRotate = oldPostRotate = originalPostRotate = postRotate;
}

/*
Call per world to define the object in each world (may be dynamic in some, static in others...)
*/
void Object::setPhysics(std::string shapeType, std::string motionTyper, PhysicsWorld* world) {
    for (unsigned int i =0; i<meshes.size(); i++) {
        //set for update method
        motionType = motionTyper;
        glm::mat4 modelNoScale = getModelNoScale();
        //for current mesh: add a physics world object
        if (motionType == "ground") {
            if (world!=NULL) {
                meshes[i].addPhysics(shapeType, motionType, world, modelNoScale, nextScale);
            }
            meshes[i].setCCD();
        } else if (motionType=="static") {
            if (world!=NULL) {
                meshes[i].addPhysics(shapeType, motionType, world, modelNoScale, nextScale);
            } 
            meshes[i].setCCD();
        } else if (motionType=="dynamic") {
            if (world!=NULL) {
                meshes[i].addPhysics(shapeType, motionType, world, modelNoScale, nextScale);
            }
            meshes[i].setCCD();
        } else {
            std::cout<<"ERROR: unrecognized motionType."<<std::endl;
            exit(EXIT_FAILURE);
        }
        if (i==meshChoice) {
            meshes[i].enablePhysics();
        }
    }
}

void Object::setCollider(Object* collideObject, bool in, float margin, bool edge, bool reset, bool freeze) {
    collisionObject temp;
    temp.object = collideObject;
    temp.in = in;
    temp.margin = margin;
    temp.edge = edge;
    temp.reset = reset;
    temp.freeze = freeze;
    colliders.push_back(temp);
}

glm::vec4 Object::worldTrans(glm::vec4 vector, glm::mat4 transform) {
    glm::vec4 where = transform * vector;
    return where;
}

glm::mat4 Object::getModel() {
    glm::mat4 newModel;
    newModel = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    newModel = glm::rotate(newModel, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    newModel = glm::rotate(newModel, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    newModel = glm::translate(newModel, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    newModel = glm::rotate(newModel, nextPostRotate.x, glm::vec3(1.0, 0.0, 0.0));
    newModel = glm::rotate(newModel, nextPostRotate.y, glm::vec3(0.0, 1.0, 0.0));
    newModel = glm::rotate(newModel, nextPostRotate.z, glm::vec3(0.0, 0.0, 1.0));
    newModel = glm::scale(newModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
    return newModel;
}

glm::mat4 Object::getModelNoScale() {
    glm::mat4 newModel;
    newModel = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    newModel = glm::rotate(newModel, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    newModel = glm::rotate(newModel, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    newModel = glm::translate(newModel, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    newModel = glm::rotate(newModel, nextPostRotate.x, glm::vec3(1.0, 0.0, 0.0));
    newModel = glm::rotate(newModel, nextPostRotate.y, glm::vec3(0.0, 1.0, 0.0));
    newModel = glm::rotate(newModel, nextPostRotate.z, glm::vec3(0.0, 0.0, 1.0));
    return newModel;
}

glm::vec4 Object::getCurrentMax() {
    glm::vec4 maxim = glm::vec4(meshes[meshChoice].maxVert[0], meshes[meshChoice].maxVert[1], meshes[meshChoice].maxVert[2], 1);
    return model * maxim;
}

glm::vec4 Object::getCurrentMin() {
    glm::vec4 minum = glm::vec4(meshes[meshChoice].minVert[0], meshes[meshChoice].minVert[1], meshes[meshChoice].minVert[2], 1);
    return model * minum;
}

glm::vec4 Object::getCurrentCenter() {
    glm::vec4 cent = glm::vec4(meshes[meshChoice].centerVert[0], meshes[meshChoice].centerVert[1], meshes[meshChoice].centerVert[2], 1);
    return model * cent;
}

glm::vec4 Object::getGlobalMax() {
    glm::vec4 maxim = glm::vec4(meshes[meshChoice].maxVert[0], meshes[meshChoice].maxVert[1], meshes[meshChoice].maxVert[2], 1);
    return maxim;
}

glm::vec4 Object::getGlobalMin() {
    glm::vec4 minum = glm::vec4(meshes[meshChoice].minVert[0], meshes[meshChoice].minVert[1], meshes[meshChoice].minVert[2], 1);
    return minum;
}

glm::vec4 Object::getGlobalCenter() {
    glm::vec4 cent = glm::vec4(meshes[meshChoice].centerVert[0], meshes[meshChoice].centerVert[1], meshes[meshChoice].centerVert[2], 1);
    return cent;
}

glm::vec4 Object::getNextMax() {
    glm::mat4 newM = getModel();
    glm::vec4 maxim = glm::vec4(meshes[meshChoice].maxVert[0], meshes[meshChoice].maxVert[1], meshes[meshChoice].maxVert[2], 1);
    return newM * maxim;
}

glm::vec4 Object::getNextMin() {
    glm::mat4 newM = getModel();
    glm::vec4 minum = glm::vec4(meshes[meshChoice].minVert[0], meshes[meshChoice].minVert[1], meshes[meshChoice].minVert[2], 1);
    return newM * minum;
}

glm::vec4 Object::getNextCenter() {
    glm::mat4 newM = getModel();
    glm::vec4 cent = glm::vec4(meshes[meshChoice].centerVert[0], meshes[meshChoice].centerVert[1], meshes[meshChoice].centerVert[2], 1);
    return newM * cent;
}
//following meant for skybox, left unfinished for more important tasks
void Object::setBrightness(glm::vec4 bright) {
    for (unsigned int i = 0; i<meshes.size(); i++) {
        //
    }
}

//moves next into old
void Object::updateStorage() {
    oldModels.push(model);
    if(oldModels.size()> maxHistory) {
        oldModels.pop();//remove front
    }
    oldTranslate = nextTranslate;
    oldRotate = nextRotate;
    oldScale = nextScale;
}

void Object::removeNextMove() {
    nextTranslate = oldTranslate;
    nextRotate = oldRotate;
    nextScale = oldScale;
}

bool Object::checkCollisions() {
    //Only checks x and z for now
    bool collided;
    //check collisions for each object
    for (unsigned int i=0; i<colliders.size(); i++) {
        //std::cout<<"Checking collisions of "<<name<<" with "<<colliders[i].object->name<<std::endl;
        //get what the next model would be (not updated yet, this decides if it should be)
        //glm::mat4 newModel = getModel();
        glm::vec4 currentMax;
        glm::vec4 currentMin;
        if (colliders[i].edge) {
            //get maxes and mins for this object after transform
            currentMax = getNextMax();
            currentMin = getNextMin();
        } else {
            currentMax = getNextCenter();
            currentMin = currentMax;
        }
        //std::cout<<"currentMax and Min "<<currentMax.x<<','<<currentMax.y<<','<<currentMax.z<<std::endl;
        //std::cout<<currentMin.x<<','<<currentMin.y<<','<<currentMin.z<<std::endl;
        //get max and min transforms for other object (or center)
        glm::vec4 otherMax, otherMin;
        //want to check if edges hit
        otherMax = colliders[i].object->getCurrentMax();
        otherMin = colliders[i].object->getCurrentMin();
        //std::cout<<"current collider max and min "<<otherMax.x<<','<<otherMax.y<<','<<otherMax.z<<std::endl;
        //std::cout<<otherMin.x<<','<<otherMin.y<<','<<otherMin.z<<std::endl;

        //need object to be inside other
        if (colliders[i].in==true && 
            (currentMax[0]>otherMax[0]-colliders[i].margin || currentMax[1]>otherMax[1]-colliders[i].margin || currentMax[2]>otherMax[2]-colliders[i].margin 
            || currentMin[0]<otherMin[0]+colliders[i].margin || currentMin[1]<otherMin[1]+colliders[i].margin || currentMin[2]<otherMin[2]+colliders[i].margin)) {
            //object should be inside object (like with the Board)
            //don't allow motion to happen: undo translate change
            collided = true;
            colliders[i].object->collidedWith = true;
            if (colliders[i].reset) {
                //resetPosition();
                reset = true;
            } else if (colliders[i].freeze) {
                frozen = true;
                meshes[meshChoice].disablePhysics();
            } else {
                removeNextMove();
            }
        //need object to be outside of other
        } else if (colliders[i].in==false && 
            ((currentMax[0]<otherMax[0]+colliders[i].margin && currentMax[1]<otherMax[1]+colliders[i].margin && currentMax[2]<otherMax[2]+colliders[i].margin) 
            && currentMin[0]>otherMin[0]-colliders[i].margin && currentMax[1]<otherMax[1]+colliders[i].margin && currentMin[2]>otherMin[2]-colliders[i].margin)) {
            //object should be outside (use for standard collisions)
            //std::cout<<name<<" ran into the object "<<colliders[i].object->name<<std::endl;
            collided = true;
            colliders[i].object->collidedWith = true;
            if (colliders[i].reset) {
                //resetPosition();
                reset = true;
            } else if (colliders[i].freeze) {
                frozen = true;
                meshes[meshChoice].disablePhysics();
            } else {
                removeNextMove();
            }
        }
    }
    return collided;
}

/*
This functions must: check physics postions, update the transformations, update the model matrix, save old model matrix
*/
void Object::updateModel() {
    if(!frozen) {
        // get the current standard world transform
        bool collidedHere = false;
        glm::mat4 tempModel;
        btVector3 temptrans;
        btTransform tempTransform;
        //check collision with special object
        collidedHere = checkCollisions();
        if (!collidedHere) {
            if (motionType == "static") {
                //set transform
                model = getModelNoScale();
                //tempTransform.setOrigin(btVector3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
                //btQuaternion quat(nextRotate.y, nextRotate.x, nextRotate.z);
                //tempTransform.setRotation(quat);
                tempTransform.setFromOpenGLMatrix(glm::value_ptr(model));
                //set in both worlds
                meshes[meshChoice].physics[0]->setMotion(tempTransform);
                //meshes[meshChoice].physics[1]->setMotion(tempTransform);
                //update local model storage ::Shoudl be what we just set
                tempModel = meshes[meshChoice].physics[0]->getCurrentTransformMat();
                //std::cout<<"Scale "<<nextScale.x<<','<<nextScale.y<<','<<nextScale.z<<std::endl;
                model = glm::scale(tempModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
            } else if (motionType == "ground") {
                //complicated: motion may break things...
                model = getModelNoScale();
                //set Transform
                tempTransform.setFromOpenGLMatrix(glm::value_ptr(model));
                //set posiiton in both worlds
                meshes[meshChoice].physics[0]->setMotion(tempTransform);
                //update local model storage ::Should be what we just set
                tempModel = meshes[meshChoice].physics[0]->getCurrentTransformMat();
                //std::cout<<"Scale "<<nextScale.x<<','<<nextScale.y<<','<<nextScale.z<<std::endl;
                model = glm::scale(tempModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
            } else if (motionType == "dynamic") {
                //update from first and only world
                tempModel = meshes[meshChoice].physics[0]->getCurrentTransformMat();
                temptrans = meshes[meshChoice].physics[0]->getCurrentTranslatebt();
                //
                nextTranslate = glm::vec3(temptrans[0], temptrans[1],temptrans[2]);
                model = glm::scale(tempModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
            } else if (motionType=="NONE") {
                //non-physcics object, do nothing
                model = getModel();
            } else {
                std::cout<<"ERROR: No matching motion type in model update. "<<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        //store any changes (or no changes)
        updateStorage();
    }
}

void Object::checkError(std::string where=" "){
    ErrorCheckValue = glGetError();
    //std::cout<<"Checking Error"<<std::endl;
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        //std::cout<<"Error occured "<<std::endl;
        fprintf(
            stderr,
            "ERROR: %s: %s \n", where.c_str(),
            gluErrorString(ErrorCheckValue)
        );
        exit(-1);
    }
}

//sets Position base don passed values
void Object::resetPosition(glm::vec3 trans, glm::vec3 rot, glm::vec3 scal) {
    //originalBlah  are invalid for this model...we have to recalulaste start based on some other object for this
    nextTranslate = trans;
    nextRotate = rot;
    nextScale = scal;
    model = getModel();
    oldModels.empty();
    oldModels.push(model);
    resetPhysics();
}

//sets position based on start position
void Object::resetPosition() {
    //std::cout<<originalTranslate.x<<','<<originalTranslate.y<<','<<originalTranslate.z<<std::endl;
    /*if(name=="assets/models/moon.obj") {
        std::cout<<"Model before in resetPosition"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }
    std::cout<<"Translate before reset "<<nextTranslate.x<<','<<nextTranslate.y<<','<<nextTranslate.z<<std::endl;*/
    model = originalModel;
    nextTranslate = oldTranslate = originalTranslate;
    nextRotate = oldRotate = originalRotate;
    nextScale = oldScale = originalScale;
    //remove this if we want replay
    oldModels.empty();
    oldModels.push(model);
    resetPhysics();
}

void Object::resetPhysics() {
    //reset each mesh
    std::cout<<"resetting "<<name<<std::endl;
    meshes[meshChoice].resetPhysics(getModelNoScale(), nextScale);//model);
}

void Object::setTransforms(glm::vec3 trans, glm::vec3 rot, glm::vec3 sca, PhysicsWorld* world, std::string moves, std::string shape) {
    nextTranslate = oldTranslate = originalTranslate = trans;
    nextRotate = oldRotate = originalRotate = rot;
    nextScale = oldScale = originalScale = sca;
    //set model matrix
    model = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    model = glm::rotate(model, nextPostRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextPostRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextPostRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
    originalModel = model;
    if (world != NULL) {
        setPhysics(shape, moves, world);
    }
}

void Object::initializeObject(){

    //buffer stuff for each subMesh, in each mesh...yay
    for (unsigned int i=0; i<meshes.size(); i++) {
        meshes[i].initializeBuffers();
        checkError("After Initializing a mesh");
    }
}

void Object::drawObject() {
    //only draw current mesh
    meshes[meshChoice].drawMeshes(model);
}

void Object::flipNormals() {
    for (unsigned int i = 0; i<meshes.size(); i++) {
        meshes[i].flipNormals();
    }
}

void Object::addMesh(std::string path, std::string filename) {
    //load another mesh and push_back
    meshes.push_back(Mesh(path, filename));
}

void Object::swapMesh(unsigned int newMesh) {
    //swap the physics object in the world
    //exchange information from one mesh to another
    //need to remove from physics world
    meshes[meshChoice].disablePhysics();
    //pull out current position
    //need to add to other physics world
    meshes[newMesh].enablePhysics();
    meshChoice = newMesh;
    resetPhysics();
    //change meshChoice
}

void Object::disablePhysics() {
    meshes[meshChoice].disablePhysics();
}

void Object::enablePhysics() {
    meshes[meshChoice].enablePhysics();
}
