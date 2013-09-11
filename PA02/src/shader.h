/*

Project 03, Graphics
Copyright 2013, Liesl Wigand

Shader class header:
    loads, and initializes the shaders

*/
// Needs glew, glut, glm? Just gl.h? order will matter in main
#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting
// #include <GL/gl.h> // not needed now

#ifndef SHADER_H_
#define SHADER_H_

class Shader {
  private:
    // handle GLuint
    GLenum kind;
    GLuint handle;

    // load from file
    char* loadShader(char* filename);
    // compile
    void compile(const char *shad);
    // create
    void createShader(GLuint shaderType, const char *shad);

  public:
    // constructor(s)
    explicit Shader(GLuint shaderType);
    Shader(GLuint shaderType, char* filename);
    ~Shader();
    // added function to move possible errors
    // from constructor to other...exceptions wouldve worked too.
    GLuint get() const;
};

#endif  // SHADER_H_
