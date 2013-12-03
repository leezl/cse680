/* 
Program class: meant to store a given program and its enabled features
*/
#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

struct Light {
    glm::vec4 pos;
    glm::vec4 amb;
    glm::vec4 diff;
    glm::vec4 spec;

    Light & operator=(Light other) {
    	pos = other.pos;
    	amb = other.amb;
    	diff = other.diff;
    	spec = other.spec;
    	return *this;
    }
    Light () {
    	pos = glm::vec4(0.0,0.0,0.0,1.0);
    	amb = glm::vec4(0.0,0.0,0.0,1.0);
    	diff = glm::vec4(0.0,0.0,0.0,1.0);
    	spec = glm::vec4(0.0,0.0,0.0,1.0);
    }
    Light (const Light& other) {
    	pos = other.pos;
    	amb = other.amb;
    	diff = other.diff;
    	spec = other.spec;
    }
};

struct LightLoc {
    GLint loc_AmbProd, loc_LightPos, loc_Shin, loc_DiffProd, loc_SpecProd;
};

class Program {
public:
    Program(bool useNormals=false, bool useColors=false, bool useTextures=false);
    //set up pointers to program variables that main updates
    void addLight(Light & lighter);
    void addView(glm::mat4 & view);
    void addProjection(glm::mat4 & projection);
    //or, set the values manually
    bool setProgramConstants();
    bool setModel(glm::mat4 * model=NULL);
	bool setView(glm::mat4 * view=NULL);
	bool setProjection(glm::mat4 * projection=NULL);
    bool setTextureSampler(GLuint * texI=NULL);
	bool setLightPosition(Light * lighter=NULL, glm::mat4 * viewer=NULL);
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
	LightLoc lightin;
	Light * light;
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