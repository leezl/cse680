/*
Storage for objects: buffer id, model matrix

Delete other load Funcitons when done picking one
*/
#include <stdio.h>
#include <iostream>
#include <vector>
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
//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "program.h"
#include "texture.h"

#ifndef OBJECT_H_
#define OBJECT_H_

/*
Need: separate draw for each material: different lighting, so: need to keep faces split by material
Keep in Assimp aiScene: store the aiScene, and iterate through it in the draw function.
*/

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

class Object {
  private:
    std::vector<GLuint> elementBuffers, geometryBuffers, normalBuffers, colorBuffers, textureBuffers;
    //aiScene * scene;
    //following are vectors of meshes: separate mesh per material
    Program * myProgram;//the program that works for this object
    std::string name;
    std::vector< Material > materials;
    std::vector< std::vector<unsigned int> > indices; //by mesh; flattens faces out
    std::vector< std::vector< glm::vec3 > > vertices;
    std::vector< std::vector< glm::vec3 > > normals;
    std::vector< std::vector< glm::vec3 > > uvs;
    std::vector< std::vector< glm::vec4 > > colors;
    std::vector< unsigned int > materialIndices;
    //Mode of draw: elements
    GLenum ErrorCheckValue;
    void checkError(std::string where);
    bool loadAssImp(std::string dir, std::string path);
  public:
    glm::mat4 model;//this objects model
    bool hasMaterials, hasTextures;//determine which shader to use
    Object();
    Object(std::string path, std::string filename);
    ~Object();
    void initializeObject();
    void cleanUp();
    void drawObject();
    void flipNormals();
    void setProgram(Program * program);
    float center[3];
    float max[3];
    float min[3];
};

#endif  // OBJECT_H_
