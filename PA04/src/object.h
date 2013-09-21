/*
Storage for objects: buffer id, model matrix

Delete other load Funcitons when done picking one
*/
#include <stdio.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct group {
  // collect faces into groups here, associate with mtl...
    std::vector < face > faces;//indices to vertices in out_vertices
    string material;
};

struct face {
    std::vector < int > vert;//indices to vertices in out_vertices
    std::vector < int > uv;
    std::vector < int > norm;
};

class indexSet {
   int vertex;
   int normal;
   int uv;
   string color;

   indexSet operator==(indexSet o){
      return vertex==o.vertex && normal==o.normal && uv==o.uv && strcmp(color,o.color)
   }
};

struct mtl{
  // These first 7 are in my file from blender
  string name;
  double Ns;
  std::vector < glm::vec3 > Ka;  // ambient
  std::vector < glm::vec3 > Kd;  // diffuse
  std::vector < glm::vec3 > Ks;  // specular
  double Ni;  // optical density: bends light or not(1.0)
  double d;  // may have -halo; dissolve factor
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
    glm::mat4 model;
    GLuint geometry;
    //reformat following to be by group ie: use indexing again?
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec2 > uvs;
    std::vector< glm::vec3 > normals;
    std::vertex< glm::vec3 > colors;
    std::vertex< int > indices;
    //need collection of material
    std::vector< mtl > materials;
    //need groups of faces per material
    std::vector< group > groups;
    //Mode of draw: vertex_arrays, elements
    int mode = 1;  // element vs array...need better way to store these
    
  public:
    Object(const char* filename);
    ~Object();
    bool loadObjectElementsColor(const char * path);
    bool loadObjectVertices(const char * path);
    bool loadObjectElements(const char * path);
    bool loadMaterial(const char* filename);
    bool initializeObject();
    bool drawObject();
    glm::mat4 getModel();
    GLuint getGeometry();

};