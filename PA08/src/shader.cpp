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
        const char *shad = loadShader((char *)"assets/shaders/140VertShader.vs");
        createShader(shaderType, shad);
    }
    else if ( shaderType == GL_FRAGMENT_SHADER ) {
        const char *shad = loadShader((char *)"assets/shaders/140FragShader.fs");
        createShader(shaderType, shad);
    }
    else {
        //what? but we don't have one of those!!!???
        std::cerr << "We don't have a default shader for "
           "anything but Vertex and Fragment. Check your shaders." 
            << shaderType << std::endl;
        const char *shad = NULL;
        createShader(shaderType, shad);
    }
}

// Constructor with filename recieved
Shader::Shader(GLenum shaderType, char* filename) {
    // load shad from file
    const char *shad = loadShader(filename);
    // create shader
    createShader(shaderType, shad);
}

//creates some basic shader stuff that is the same across types
void Shader::createShader(GLenum shaderType, const char *shad) {
    try {
        if ( shad == NULL ) {
	    throw 10;
        }
    }
    catch (int e) {
        std::cerr << "Failed to load shader." << std::endl;
    }
    kind = shaderType;
    //create Shader
    handle = glCreateShader(shaderType);
    compile(shad);
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
        std::string opps;
        if (kind == GL_FRAGMENT_SHADER) {
            opps = "fragment";
        } else {
            opps = "vertex";
        }
        std::cerr << "[F] FAILED TO COMPILE SHADER! " << opps << " " << kind << std::endl;
        GLint infoLogLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(handle, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Compilation error in shader %s: %s\n", opps.c_str(), strInfoLog);
    }
    delete[] shad;
}
 
/*
loadShader
P01
input: filename
output: char * containing shader loaded from filename
 */
// Loads a shader from a file into a string
char* Shader::loadShader(char* filename) {
    //open file, check exists and open
    std::ifstream shaderfile;
    char* shadercode=NULL;
    shaderfile.open(filename);
    if (!shaderfile.good())
    {
	std::cerr<<"Shader file "<<filename<<" failed to open."<<std::endl;
	return shadercode;
    }
    //find length of file
    long length;
    shaderfile.seekg(0, shaderfile.end);
    length = shaderfile.tellg();
    shaderfile.seekg(0, shaderfile.beg);

    //create new char of the length
    shadercode = new char[length+1];

    int i = 0;
    //load shader into car
    while(shaderfile.good())
    {
      shadercode[i] = shaderfile.get();
      if(!shaderfile.eof()){
          i++;
      }
    }
    //termination symbol?
    shadercode[i]=0;
    //std::cout<<std::endl<<shadercode<<std::endl;//debug print

    //return
    return shadercode;
}

GLuint Shader::get() const {
    return handle;
}

void Shader::deleteShader() {
    glDeleteShader(handle);
}

Shader::~Shader() {
    glDeleteShader(handle);
}
