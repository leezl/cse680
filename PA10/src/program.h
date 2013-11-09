/* 
Program class: meant to store a given program and its enabled features
*/
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#ifndef PROGRAM_H_
#define PROGRAM_H_

struct Light {
    glm::vec4 pos;
    glm::vec4 amb;
    glm::vec4 diff;
    glm::vec4 spec;
    float constantAtten, linearAtten, quadraticAtten;
    float spotCutoff, spotExp;
    glm::vec4 spotDir;
    bool on;

    Light & operator=(Light other) {
        pos = other.pos;
        amb = other.amb;
        diff = other.diff;
        spec = other.spec;
        spotDir = other.spotDir;
        constantAtten = other.constantAtten;
        linearAtten = other.linearAtten;
        quadraticAtten = other.quadraticAtten;
        spotCutoff = other.spotCutoff;
        spotExp = other.spotExp;
        on = other.on;
        return *this;
    }
    Light () {
        pos = glm::vec4(0.0,0.0,0.0,0.0);
        amb = glm::vec4(0.0,0.0,0.0,0.0);
        diff = glm::vec4(0.0,0.0,0.0,0.0);
        spec = glm::vec4(0.0,0.0,0.0,0.0);
        spotDir = glm::vec4(0.0,0.0,0.0,0.0);
        constantAtten = 1.0;
        linearAtten = quadraticAtten = 0.0;
        spotCutoff = 360.0;
        spotExp = 0.0;
        on = true;
    }
    Light (const Light& other) {
        pos = other.pos;
        amb = other.amb;
        diff = other.diff;
        spec = other.spec;
        spotDir = other.spotDir;
        constantAtten = other.constantAtten;
        linearAtten = other.linearAtten;
        quadraticAtten = other.quadraticAtten;
        spotCutoff = other.spotCutoff;
        spotExp = other.spotExp;
        on = other.on;
    }
};

struct LightPos {
    GLint locs[10];
};

struct MaterialPos {
    //ambient, diffuse, specular, shininess
    GLint locs[4];
};

class Program {
public:
    Program(bool useNormals=false, bool useColors=false, bool useTextures=false);
    //set up pointers to program variables that main updates
    void addLight(Light & lighter, int index=0);
    void addView(glm::mat4 & view);
    void addProjection(glm::mat4 & projection);
    //or, set the values manually
    bool setProgramConstants();
    bool setModelView(glm::mat4 * modeler=NULL, glm::mat4 * viewer=NULL);
    bool setModel(glm::mat4 * model=NULL);
	bool setView(glm::mat4 * view=NULL);
	bool setProjection(glm::mat4 * projection=NULL);
    bool setTextureSampler(GLuint * texI=NULL);
	bool setLightPosition(Light * lighter=NULL, glm::mat4 * viewer=NULL, int index=0);
    bool setLights();
	//start and stop: enables and useProgram
	bool startProgram();
	bool stopProgram();
	~Program();
	//should probably make a function to handle these when drawing...
	GLint loc_position;
	GLint loc_color;
	GLint loc_normal;
	GLint loc_uv;
    GLint loc_tex;
    GLint loc_timat;
    Light offLight;
	Light* lights[4];
    LightPos lightPos[4];
    MaterialPos materialPosition;
	glm::mat4 * view;
	glm::mat4 * projection;
    GLuint program;
    bool needsNormals;
    bool needsColors;
    bool needsTextures;
private:	
	//may not have all of the following
	// attribute locations
	GLint loc_mmat;
	GLint loc_vmat;
	GLint loc_pmat;
	bool compileShaders();
	bool loadLocations();
};

#endif  // PROGRAM_H_