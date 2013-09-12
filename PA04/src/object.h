/*
Storage for objects: buffer id, model matrix
*/

class Objects {
  private:
    glm::mat4 model;
    GLuint geometry;
    
  public:
    Objects();
    ~Objects();
    glm::mat4 getModel();
    GLuint getGeometry();

};
