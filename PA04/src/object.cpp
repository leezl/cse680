/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/

Object::Object();
Object::~Object();
glm::mat4 Object::getModel();
GLuint Object::getGeometry();