/*
Storage for objects: buffer id, model matrix

Delete other load Funcitons when done picking one
*/
#include <stdio.h>
#include <iostream>
#include <vector>
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

#ifndef OBJECT_H_
#define OBJECT_H_

/*
Need: separate draw for each material: different lighting, so: need to keep faces split by material
Keep in Assimp aiScene: store the aiScene, and iterate through it in the draw function.
*/

struct Light {
    glm::vec4 lightPos;
    glm::vec4 lightAmb;
    glm::vec4 lightDiff;
    glm::vec4 lightSpec;
};
struct LightLoc {
    GLint loc_AmbProd, loc_LightPos, loc_Shin, loc_DiffProd, loc_SpecProd;
};


class Object {
  private:
    std::vector<GLuint> elementBuffers, geometryBuffers, normalBuffers, colorBuffers, textureBuffers;
    aiScene * scene;
    std::vector< std::vector<unsigned short> > indices; //by mesh; flattens faces out
    //Mode of draw: elements
    GLenum ErrorCheckValue;
    void checkError();
    bool loadAssImp(std::string path);
    
  public:
    Object(std::string path, std::string filename);
    ~Object();
    void initializeObject();
    void cleanUp();
    void drawObject(GLint position, GLint normal, GLint uv, GLint color, GLint shin, Light light, LightLoc lightin);
    float center[3];
    float max[3];
    float min[3];
};

#endif  // OBJECT_H_