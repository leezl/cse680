/*

Project 03, Graphics
Copyright 2013, Liesl Wigand

Shader class header:
    loads, and initializes the shaders

*/
#include <fstream>
#include <iostream>
#include "shader.h"
 
// Constructor with default shader loaded (still needs type)
Shader::Shader(GLenum shaderType) {
    // check type
    if ( shaderType == GL_VERTEX_SHADER ) {
        // store correct default
      const char *shad = loadShader((char *)"assets/shader/ptVertShader.txt");
    }
    else if ( shaderType == GL_VERTEX_SHADER ) {
      const char *shad = loadShader((char *)"assets/shader/ptFragShader.txt");
    }
    else {
        //what? but we don't have one of those!!!???
        std::cerr << "We don't have a default shader for "
           "anything but Vertex and Fragment. Check your shaders." << std::endl;
        const char *shad = NULL;
    }
    try {
        if ( shad == NULL ) {
	    throw 10;
        }
    }
    catch (int e) {
        std::cerr << "Failed to load shader." << std::endl;
    }
    createShader(shaderType, shad);
}

// Constructor with filename recieved
Shader::Shader(GLenum shaderType, char* filename) {
    // load shad from file
    const char *shad = loadShader(filename);
    // create shader
    createShader(shaderType, shad);
}

//compiles shader, used in constructor (we assume you'll use what you load now)
void Shader::compile(const char *shad) {
    // compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(handle, 1, &shad, NULL);
    glCompileShader(handle);
    // check the compile status
    glGetShaderiv(handle, GL_COMPILE_STATUS, &shader_status);
    try{
        if ( !shader_status ) {
            throw 15;
        }
    }
    catch (int e) {
      std::cerr << "[F] FAILED TO COMPILE SHADER! " << ref << std::endl;
    }
}

//creates some basic shader stuff that is the same across types
void Shader::createShader(GLenum shaderType, const char *shad) {
    ref = shaderType;
    //create Shader
    handle = glCreateShader(shaderType);
    compile(shad);
}
 
/*
loadShader
P01
input: filename
output: char * containing shader loaded from filename
 */
// Loads a shader from a file into a string
char* Shader::loadShader(char* filename) {
    // open file, check exists and open
    std::ifstream shaderfile;
    char* shadercode;
    shaderfile.open(filename);
    if ( !shaderfile.good() ) {
        std::cerr << "Shader file " << filename << " failed to open." << Sstd::endl;
        return shadercode;
    }
    // find length of file
    long length;
    shaderfile.seekg(0, shaderfile.end);
    length = shaderfile.tellg();
    shaderfile.seekg(0, shaderfile.beg);

    // create new char of the length
    shadercode = new char[length];

    int i = 0;
    // load shader into car
    while ( shaderfile.good() ) {
      shadercode[i] = shaderfile.get();
      if ( !shaderfile.eof() ) {
          i++;
      }
    }
    // termination symbol?
    shadercode[i]=0;
    // std::cout<<std::endl<<shadercode<<std::endl;//debug print

    // return
    return shadercode;
}
