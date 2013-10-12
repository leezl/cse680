/*
This is intended to house texture stuff. may be large or small depending on library used.
*/
#include <iostream>
#include <string>
#include <il.h>

class Texture{
public:
	Texture(std::string filename);
	checkErrors();
	~Texture();

private:
	ILuint texid; /* ILuint is a 32bit unsigned integer.
	Variable texid will be used to store image name. */
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
	it can have different value (because it's just typedef of unsigned char), but this sould be
	avoided.
	Variable success will be used to determine if some function returned success or failure. */
	GLuint image;
};