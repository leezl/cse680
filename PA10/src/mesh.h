
/*
Thinking about breaking object into another class, not sure how to split it yet...
*/
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <algorithm> 
#include <typeinfo>
#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/cexport.h>         // because we want a copy of the scene...not great

#include "program.h"
#include "objectPhysics.h"
#include "texture.h"

class Object;

struct Material {
    std::string name;
    int shadingMode;
    std::map< aiTextureType, std::vector< Texture > > textureFiles; 
    float refraction;
    float shine;
    glm::vec4 amb;
    glm::vec4 diff;
    glm::vec4 spec;
};

struct collisionObject {
    //object we're checking collisions with (using max and min only for now)
    Object * object;
    //whetehr we want to be inside or outside
    bool in;
    //check collision with edges, or with center of this object
    bool edge;
    //whether we reset when we colldie or just move to previous position (don't allow next move)
    bool reset;
    //how much space we want to miss the other object by: useful if object max and min not exactly right
    float margin;

    collisionObject(){
        object = NULL;
        in = true;
        edge = true;
        reset = false;
        margin = 0.0;
    }
    collisionObject(const collisionObject& o) {
        object=o.object;
        in=o.in;
        edge = o.edge;
        reset = o.reset;
        margin = o.margin;
    }
    collisionObject operator=(collisionObject o){
        object = o.object;
        in = o.in;
        edge = o.edge;
        reset = o.reset;
        margin = o.margin;
        return *this;
    }
};

/*
We want this to make swapping models easier:
    *needs indices
    *needs vertices
    *needs normals
    *needs colors
    *needs uvs
    *needs textures
    *needs physics stuff
*/
class SubMesh {
  public:
    GLuint elementBuffer, geometryBuffer, normalBuffer, colorBuffer, textureBuffer;
    Program * program;
    std::vector< unsigned int > indices;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec3 > uvs;
    std::vector< glm::vec4 > colors;
    unsigned int materialIndex;
    bool hasMaterials, hasTextures;

    SubMesh(){
        program=NULL;
        materialIndex = -1;
        hasMaterials = hasTextures = false;
    }
    SubMesh(const SubMesh& o) {
        program = o.program;
        indices = o.indices;
        vertices = o.vertices;
        normals = o.normals;
        uvs = o.uvs;
        colors = o.colors;
        materialIndex = o.materialIndex;
        hasMaterials = o.hasMaterials;
        hasTextures = o.hasTextures;
    }
    SubMesh operator=(SubMesh o){
        program = o.program;
        indices = o.indices;
        vertices = o.vertices;
        normals = o.normals;
        uvs = o.uvs;
        colors = o.colors;
        materialIndex = o.materialIndex;
        hasMaterials = o.hasMaterials;
        hasTextures = o.hasTextures;
        return *this;
    }
    ~SubMesh() {
        //do not delete Program here. Manual delete.
    }
    /*drawMesh(glm::mat4 model) {

    }*/
};

class Mesh {
  public:
    //contains all the data for an object loaded from one file
    std::string name;//filename
    std::vector< Material > materials;
    std::vector< SubMesh > meshes;
    glm::vec3 centerVert;
    glm::vec3 maxVert;
    glm::vec3 minVert;
    //ObjectPhysics* physics[2];
    std::vector< ObjectPhysics* > physics;
    unsigned totalTriangles, totalVerts;
    bool notStatic;

    Mesh(std::string name="defaultName");
    Mesh(glm::vec3 maxi, glm::vec3 mini, std::string newName="defaultName");
    Mesh(std::string path, std::string filename);
    ~Mesh();
    bool loadAssImp(std::string dir, std::string path);
    void cleanUp();
    bool setProgram(std::vector< Program* > programs);
    void flipNormals();

    //void setPhysics(std::string shapeType, std::string motionType, PhysicsWorld* worlds[2]);
    void addPhysics(std::string shapeType, std::string motionType, PhysicsWorld* world, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale);
    void resetPhysics(glm::mat4 model);
    void disablePhysics();
    void enablePhysics();
    void setCCD(int dist=1);

    void initializeBuffers();
    void drawMeshes(glm::mat4 model);
    void checkError(std::string where);
};