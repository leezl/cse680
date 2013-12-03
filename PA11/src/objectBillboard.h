
#ifndef OBJECT_BILLBOARD_
#define OBJECT_BILLBOARD_


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  


#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting

#include <il.h>
#include <ilu.h>



class ObjectBillboard
{
	public:
	
	//Constructor
	//Create program, vertice, and uv buffer.
	// - Program buffer used in drawObject (Render callback)
	// - Vertice and uv buffer used in updateModel (Idle callback)
	ObjectBillboard(std::string filename);
	
	//Update vertice and uv coordinates and load data into buffer
	void updateModel(const char * text, int x, int y, int size);
	
	//Render for display of text
	void drawObject();



	private:
	GLuint textureID;
	GLuint programID;
	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint uniformID;

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;


	void loadTexture(std::string filename);
	GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
};







#endif  // OBJECT_BILLBOARD_
