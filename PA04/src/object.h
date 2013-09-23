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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef OBJECT_H_
#define OBJECT_H_

struct indexSet {
   int vertex;
   int normal;
   int uv;
   std::string color;
};

struct mtl{
  // These first 7 are in my file from blender
  std::string name;
  float Ns;
  glm::vec3 Ka;  // ambient
  glm::vec3 Kd;  // diffuse
  glm::vec3 Ks;  // specular
  float Ni;  // optical density: bends light or not(1.0)
  float d;  // may have -halo; dissolve factor
  int illum;  // illumination model
  // The following can be in mtl files, but aren't in mine
  //std::vector < glm::vec3 > Tf;
  //double Tr; may replace d...
  //int sharpness;
  //double Ns; 
  // Also may be map_<item> for Ka, Ks, Kd, d, Ns
};

class Object {
  private:
    glm::mat4 model = glm::mat4(1.0f);
    GLuint elementBuffer, geometryBuffer, normalBuffer, colorBuffer, textureBuffer;
    //reformat following to be by group ie: use indexing again?
    bool hasVert=false, hasTex=false, hasNorm = false, hasColor=false;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec2 > uvs;  // not used
    std::vector< glm::vec3 > normals;  // not used
    std::vector< glm::vec4 > colors;
    std::vector< GLubyte > indices;
    //need collection of material
    std::vector< mtl* > materials;
    //Mode of draw: vertex_arrays, elements
    int mode = 1;  // element vs array...need better way to store these
    GLenum ErrorCheckValue;
    
  public:
    Object(const char* filename);
    ~Object();
    bool loadObjectElementsColor(const char * path);
    bool loadMaterial(const char* filename);
    void initializeObject();
    void cleanUp();
    void drawObject(GLint, GLint, GLint, GLint);
};

#endif  // OBJECT_H_