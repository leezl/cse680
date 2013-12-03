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
    nextTranslate = oldTranslate = originalTranslate = glm::vec3(0.0,0.0,0.0);
    nextRotate = oldRotate = originalRotate = glm::vec3(0.0,0.0,0.0);
    nextScale = oldScale = originalScale = glm::vec3(1.0,1.0,1.0);
    //set model matrix
    model = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    model = glm::scale(model, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
    originalModel = model;
    maxHistory=2;
}

Object::~Object(){
    //had cleanUp here, but may not have initialized so...
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

/*
Call per world to define the object in each world (may be dynamic in some, static in others...)
*/
void Object::setPhysics(std::string shapeType, std::string motionTyper, PhysicsWorld* worlds[2]) {
    for (unsigned int i =0; i<meshes.size(); i++) {
        //set for update method
        motionType = motionTyper;
        //for current mesh: add a physics world object
        if (motionType == "ground") {
            meshes[i].addPhysics(shapeType, motionType, worlds[0], nextTranslate, nextRotate, nextScale);
            meshes[i].addPhysics(shapeType, motionType, worlds[1], nextTranslate, nextRotate, nextScale);
            meshes[i].setCCD();
        } else if (motionType=="static") {
            meshes[i].addPhysics(shapeType, motionType, worlds[0], nextTranslate, nextRotate, nextScale);
            meshes[i].addPhysics("convex", "dynamic", worlds[1], nextTranslate, nextRotate, nextScale);
            meshes[i].setCCD();
        } else if (motionType=="dynamic") {
            meshes[i].addPhysics(shapeType, motionType, worlds[0], nextTranslate, nextRotate, nextScale);
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

void Object::setCollider(Object* collideObject, bool in, float margin, bool edge, bool reset) {
    collisionObject temp;
    temp.object = collideObject;
    temp.in = in;
    temp.margin = margin;
    temp.edge = edge;
    temp.reset = reset;
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
    newModel = glm::scale(newModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
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
    bool collided = false;
    bool resetThis = false;
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
            (currentMax[0]>otherMax[0]-colliders[i].margin || currentMax[2]>otherMax[2]-colliders[i].margin 
            || currentMin[0]<otherMin[0]+colliders[i].margin || currentMin[2]<otherMin[2]+colliders[i].margin)) {
            //object should be inside object (like with the Board)
            //don't allow motion to happen: undo translate change
            collided = true;
            colliders[i].object->collidedWith = true;
            if (colliders[i].reset) {
                //std::cout<<name<<" left container "<<colliders[i].object->name<<std::endl;
                resetThis = true;
            }
        //need object to be outside of other
        } else if (colliders[i].in==false && 
            ((currentMax[0]<otherMax[0]+colliders[i].margin && currentMax[2]<otherMax[2]+colliders[i].margin) 
            && currentMin[0]>otherMin[0]-colliders[i].margin && currentMin[2]>otherMin[2]-colliders[i].margin)) {
            //object should be outside (use for standard collisions)
            //std::cout<<name<<" ran into the object "<<colliders[i].object->name<<std::endl;
            collided = true;
            colliders[i].object->collidedWith = true;
            if (colliders[i].reset) {
                //std::cout<<name<<" collided with "<<colliders[i].object->name<<std::endl;
                resetThis = true;
            }
        }
    }
    if ( collided == true ) {
        //undo the nextTranslate, Rotate Scale etc, since they will move us out of bounds
        if (resetThis) {
            //std::cout<<"resetting "<<name<<std::endl;
            resetPosition();
        } else {
            removeNextMove();
        }
    }
    return collided;
}

/*
This functions must: check physics postions, update the transformations, update the model matrix, save old model matrix
*/
void Object::updateModel() {
    // get the current standard world transform
    bool collided = false;
    glm::mat4 tempModel;
    btVector3 temptrans;
    btTransform tempTransform;
    //check collision with special object
    collided = checkCollisions();
    /*if(name=="assets/models/moon.obj") {
        std::cout<<"Model before in updateModel"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
    if (!collided) {
        if (motionType == "static") {

            //need both worlds: update y from second, update from here into first
            tempTransform = meshes[meshChoice].physics[0]->getCurrentTransformbt();
            temptrans = meshes[meshChoice].physics[1]->getCurrentTranslatebt();
            //have correct y, now update other
            nextTranslate = glm::vec3(nextTranslate.x, temptrans[1], nextTranslate.z);
            //set transform
            tempTransform.setOrigin(btVector3(nextTranslate.x, temptrans[1], nextTranslate.z));
            //set in both worlds
            meshes[meshChoice].physics[0]->setMotion(tempTransform);
            meshes[meshChoice].physics[1]->setMotion(tempTransform);
            //update local model storage ::Shoudl be what we just set
            /*if(name=="assets/models/paddle2Blue.obj") {
                std::cout<<"Model before, in static loop in updateModel"<<std::endl;
                for(int i=0; i<4; i++) {
                    for(int j=0; j<4; j++) {
                        std::cout<<model[i][j]<<',';
                    }
                    std::cout<<std::endl;
                }
            }*/
            tempModel = meshes[meshChoice].physics[0]->getCurrentTransformMat();
            /*if(name=="assets/models/paddle2Blue.obj") {
                std::cout<<"Model after getCurrentTransform, before scale in updateModel"<<std::endl;
                for(int i=0; i<4; i++) {
                    for(int j=0; j<4; j++) {
                        std::cout<<tempModel[i][j]<<',';
                    }
                    std::cout<<std::endl;
                }
            }*/
            //std::cout<<"Scale "<<nextScale.x<<','<<nextScale.y<<','<<nextScale.z<<std::endl;
            model = tempModel;//glm::scale(tempModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
            /*if(name=="assets/models/paddle2Blue.obj") {
                std::cout<<"Model after getCurrentTransform in updateModel"<<std::endl;
                for(int i=0; i<4; i++) {
                    for(int j=0; j<4; j++) {
                        std::cout<<model[i][j]<<',';
                    }
                    std::cout<<std::endl;
                }
            }*/
        } else if (motionType == "ground") {
            //don't care about motion: shouldn't move
        } else if (motionType == "dynamic") {
            /*if(name=="assets/models/moon.obj") {
                std::cout<<"Model inside no collision check updateModel"<<std::endl;
                for(int i=0; i<4; i++) {
                    for(int j=0; j<4; j++) {
                        std::cout<<model[i][j]<<',';
                    }
                    std::cout<<std::endl;
                }
            }*/
            //update from first and only world
            tempModel = meshes[meshChoice].physics[0]->getCurrentTransformMat();
            temptrans = meshes[meshChoice].physics[0]->getCurrentTranslatebt();
            //
            nextTranslate = glm::vec3(temptrans[0], temptrans[1],temptrans[2]);
            model = tempModel;//glm::scale(tempModel, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
            /*if(name=="assets/models/moon.obj") {
                std::cout<<"Model after update from physics world updateModel"<<std::endl;
                for(int i=0; i<4; i++) {
                    for(int j=0; j<4; j++) {
                        std::cout<<model[i][j]<<',';
                    }
                    std::cout<<std::endl;
                }
            }*/
        } else if (motionType=="NONE") {
            //non-physcics object, do nothing
            model = getModel();
        } else {
            std::cout<<"ERROR: No matching motion type in model update. "<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model after in updateModel"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
    //store any changes (or no changes)
    updateStorage();
    /*if(name=="assets/models/moon.obj") {
        std::cout<<"Model after storage in updateModel"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
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
    /*if(name=="assets/models/moon.obj") {
        std::cout<<"Model after in resetPosition"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }
    std::cout<<"Translate after reset "<<nextTranslate.x<<','<<nextTranslate.y<<','<<nextTranslate.z<<std::endl;*/
}

void Object::resetPhysics() {
    //reset each mesh
    std::cout<<"resetting "<<name<<std::endl;
    meshes[meshChoice].resetPhysics(model);
}

void Object::setTransforms(glm::vec3 trans, glm::vec3 rot, glm::vec3 sca, PhysicsWorld* worlds[2], std::string moves, std::string shape) {
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model before in setTransforms"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
    nextTranslate = oldTranslate = originalTranslate = trans;
    nextRotate = oldRotate = originalRotate = rot;
    nextScale = oldScale = originalScale = sca;
    //set model matrix
    model = glm::rotate(glm::mat4(1.0f), nextRotate.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, nextRotate.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, nextRotate.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(nextTranslate.x, nextTranslate.y, nextTranslate.z));
    model = glm::scale(model, glm::vec3(nextScale.x, nextScale.y, nextScale.z));
    originalModel = model;
    if (worlds != NULL) {
        setPhysics(shape, moves, worlds);
    }
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model after in setTransforms"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
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
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model before in drawMeshes"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
    meshes[meshChoice].drawMeshes(model);
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model after in drawMeshes"<<std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
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
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model Before Reset "<<std::endl;
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
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
    /*if(name=="assets/models/paddle2Blue.obj") {
        std::cout<<"Model After Reset "<<std::endl;
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                std::cout<<model[i][j]<<',';
            }
            std::cout<<std::endl;
        }
    }*/
}
