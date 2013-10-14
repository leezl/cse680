/*
This is intended to house texture stuff. may be large or small depending on library used.
*/
#include "texture.h"

Texture::Texture(std::string filename) {
	//ilInit();//initialize devil
    //iluInit();//more
    //ilutRenderer(ILUT_OPENGL);
	ILboolean success=false; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
	it can have different value (because it's just typedef of unsigned char), but this sould be
	avoided.
	Variable success will be used to determine if some function returned success or failure. */
    ilGenImages(1, &texid); /* Generation of one image name */
    ilBindImage(texid); /* Binding of image name */
    try {
    	success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.jpg" */
    	checkErrors();
    }
    catch (std::exception& e) {
    	std::cerr<<"Texture Image Loading Failed..."<<e.what()<<std::endl;
    	exit(EXIT_FAILURE);
    }
    //check for error
    if (success) {
    	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
        unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
        if (!success)
	    {
	        /* Error occured */
	        std::cout<<"Failed to convert texture image. "<<filename<<std::endl;
	        exit(EXIT_FAILURE);
	    }
	    glGenTextures(1, &image); /* Texture name generation */
	    glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
	    //should set these from file if given...default for now
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for magnification filter */
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for minifying filter */
	    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
	    	ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
	    	ilGetData()); /* Texture specification */
	    std::cout<<"generated the texture"<<std::endl;
    } else {
	    /* Error occured */
	    std::cout<<"Failed to load texture image. "<<filename<<std::endl;
	    exit(EXIT_FAILURE);
    }
}

void Texture::bindTexture() {
	//is this all we need to do before drawing?
	glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
}

void Texture::checkErrors() {
    ILenum Error;
    std::cout<<"Checking Errors..."<<std::endl;
	while ((Error = ilGetError()) != IL_NO_ERROR) { 
	    printf("%d: %s/n", Error, iluErrorString(Error)); 
	} 
}

Texture::~Texture() {
	glDeleteTextures(1, &image);
	ilDeleteImages(1, &texid);
}