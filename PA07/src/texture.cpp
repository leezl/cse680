/*
This is intended to house texture stuff. may be large or small depending on library used.
*/

Texture::Texture() {

}

Texture::checkErrors() {
    ILenum Error;
	while ((Error = ilGetError()) != IL_NO_ERROR) { 
	    printf("%d: %s/n", Error, iluErrorString()); 
	} 
}

Texture::~Texture() {

}