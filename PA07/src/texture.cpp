/*
This is intended to house texture stuff. may be large or small depending on library used.
*/

Texture::Texture(std::string filename) {
    ilGenImages(1, &texid); /* Generation of one image name */
    ilBindImage(texid); /* Binding of image name */
    success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.jpg" */
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
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for magnification filter */
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for minifying filter */
	    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
	    	ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
	    	ilGetData()); /* Texture specification */
    } else {
	    /* Error occured */
	    std::cout<<"Failed to load texture image. "<<filename<<std::endl;
	    exit(EXIT_FAILURE);
    }
    ilDeleteImages(1, &texid);
}

Texture::checkErrors() {
    ILenum Error;
	while ((Error = ilGetError()) != IL_NO_ERROR) { 
	    printf("%d: %s/n", Error, iluErrorString()); 
	} 
}

Texture::~Texture() {
	glDeleteTextures(1, &image);
}