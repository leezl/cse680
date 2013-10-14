/*
This is intended to house texture stuff. may be large or small depending on library used.
*/
#include <iostream>
#include <string>
#include <exception>

#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting

#include <il.h>
#include <ilu.h>
//#include <ilut.h>

class Texture{
public:
	Texture();
	Texture(std::string filename);
	Texture(const Texture& other) {
		image = other.image;
	}
	void bindTexture();
	void checkErrors();
	void cleanUp();
	~Texture();
	Texture & operator=(Texture other) {
		image = other.image;
		return *this;
	}
	bool operator==(const Texture &rhs) {
		if (image==rhs.image) return true;
		return false;
	}
	//int textSize=64;
	GLubyte textImg[64][64][3];
	ILuint texid; /* ILuint is a 32bit unsigned integer.
	Variable texid will be used to store image name. */
	GLuint image;
};