/*
Storage for objects: buffer id, model matrix
*/
#include <stdio.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object {
  private:
    glm::mat4 model;
    GLuint geometry;
    std::vector < glm::vec3 > out_vertices;
    std::vector < glm::vec2 > out_uvs;
    std::vector < glm::vec3 > out_normals;
    
  public:
    Object(const char* filename);
    ~Object();
    bool loadObject((const char * path,
                        std::vector < glm::vec3 > & out_vertices,
                        std::vector < glm::vec2 > & out_uvs,
                        std::vector < glm::vec3 > & out_normals);
    glm::mat4 getModel();
    GLuint getGeometry();

};
