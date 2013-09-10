/*

Project 03, Graphics
Copyright 2013, Liesl Wigand

Shader class header:
    loads, and initializes the shaders

*/
// Needs glew, glut, glm? Just gl.h? order will matter in main
#include <GL/gl.h>

#ifndef SHADER_H_
#define SHADER_H_

class Shader {
  private:
    // handle GLuint
    GLenum ref;
    GLuint handle;

    // initialize the shader class here not constructor
    bool init();
    // load from file
    char* loadShader(char* filename);
    // compile
    void compile();
    // create
    void createShader(GLenum shaderType);

  public:
    // constructor(s)
    explicit Shader(GLenum shaderType);
    Shader(GLenum shaderType, char* filename);
    ~Shader();
    // added function to move possible errors
    // from constructor to other...exceptions wouldve worked too.
    init();
    GLuint get() const;
};

#endif  // SHADER_H_
