/*
Storage for objects: buffer id, model matrix
*/

class Object {
  private:
    glm::mat4 model;
    GLuint geometry;
    
  public:
    Object();
    ~Object();
    glm::mat4 getModel();
    GLuint getGeometry();

};
