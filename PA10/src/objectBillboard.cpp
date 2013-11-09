
#include "objectBillboard.h"


//Purpose - Create text display using texture mapping
//Procedure:
//	1.)  Obtains the textureID
//	2.)  Obtains programID after compiling and loading vertex and fragment shader
//	3.)
//
//Usage - Put in main.cpp:
//	1.)  Create billboard object ()
//	2.)  Update vertex in idlefunction callback ()
//	3.)  Draw onto screen in render callback ()



//-------------------------------------------------------------------------------
//Constructor
//1.)  Load texture
//2.)  Create buffer ID for vertex and uv coordinates
//3.)  Create program buffer ID with shaders
//4.)  Create uniform ID
//-------------------------------------------------------------------------------
ObjectBillboard::ObjectBillboard(std::string filename)
{
	//Load Texture
	loadTexture(filename);


	// Initialize VBO
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &uvBufferID);


	// Initialize Shader
	programID = loadShaders( "assets/scoreboard/TextVertexShader.vertexshader", "assets/scoreboard/TextVertexShader.fragmentshader" );

	// Initialize uniforms' IDs
	uniformID = glGetUniformLocation( programID, "myTextureSampler" );


}
//-------------------------------------------------------------------------------




//-------------------------------------------------------------------------------
//Load Texture Image and return Texture ID in GPU Buffer.
//Code copied from texture.cpp
//-------------------------------------------------------------------------------
void checkErrors() {
    ILenum Error;
    std::cout<<"Checking Errors..."<<std::endl;
	while ((Error = ilGetError()) != IL_NO_ERROR) { 
	    printf("%d: %s/n", Error, iluErrorString(Error)); 
	} 
}
void ObjectBillboard::loadTexture(std::string filename)
{


    // Create one OpenGL texture
	ILuint texid;
	

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
    	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); /* Convert every colour component into
        unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
        if (!success)
	    {
	        /* Error occured */
	        std::cout<<"Failed to convert texture image. "<<filename<<std::endl;
	        exit(EXIT_FAILURE);
	    }
	    glGenTextures(1, &textureID); /* Texture name generation */
	    glBindTexture(GL_TEXTURE_2D, textureID); /* Binding of texture name */

	   
	    //glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData()); /* Texture specification */
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData()); /* Texture specification */


	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for magnification filter */
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
	    interpolation for minifying filter */

	    checkErrors();
    } else {
	    /* Error occured */
	    std::cout<<"Failed to load texture image. "<<filename<<std::endl;
	    exit(EXIT_FAILURE);
    }
    ilDeleteImages(1, &texid);//?

}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//Load Shaders (Vertice and Fragment) and get programID
//-------------------------------------------------------------------------------
GLuint ObjectBillboard::loadShaders(const char * vertex_file_path,const char * fragment_file_path)
{

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if(VertexShaderStream.is_open()){
                std::string Line = "";
                while(getline(VertexShaderStream, Line))
                        VertexShaderCode += "\n" + Line;
                VertexShaderStream.close();
        }else{
                printf("Impossible to open %s. Are you in the right directory ? ", vertex_file_path);
                getchar();
                return 0;
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if(FragmentShaderStream.is_open()){
                std::string Line = "";
                while(getline(FragmentShaderStream, Line))
                        FragmentShaderCode += "\n" + Line;
                FragmentShaderStream.close();
        }



        GLint Result = GL_FALSE;
        int InfoLogLength;



        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertex_file_path);
        char const * VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
                std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
                glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
                printf("%s\n", &VertexShaderErrorMessage[0]);
        }



        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragment_file_path);
        char const * FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
                std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
                glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
                printf("%s\n", &FragmentShaderErrorMessage[0]);
        }



        // Link the program
        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
                std::vector<char> ProgramErrorMessage(InfoLogLength+1);
                glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
                printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
}
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//Update vertice and uv coordinates based on string variable
//-------------------------------------------------------------------------------
void ObjectBillboard::updateModel(const char * text, int x, int y, int size)
{
	//Debug	
	//printf("%s \n",text);

        unsigned int length = strlen(text);

	//Clean out vertice an uv vector
	vertices.erase(vertices.begin(), vertices.end());
	UVs.erase(UVs.begin(), UVs.end());

    // Fill buffers
    for ( unsigned int i=0 ; i<length ; i++ ){
            
            glm::vec2 vertex_up_left    = glm::vec2( x+i*size     , y+size );
            glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size );
            glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y      );
            glm::vec2 vertex_down_left  = glm::vec2( x+i*size     , y      );

            vertices.push_back(vertex_up_left   );
            vertices.push_back(vertex_down_left );
            vertices.push_back(vertex_up_right  );

            vertices.push_back(vertex_down_right);
            vertices.push_back(vertex_up_right);
            vertices.push_back(vertex_down_left);

            char character = text[i];
            float uv_x = (character%16)/16.0f;
            float uv_y = (character/16)/16.0f;

            glm::vec2 uv_up_left    = glm::vec2( uv_x           , 1.0f - uv_y );
            glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, 1.0f - uv_y );
            glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, 1.0f - (uv_y + 1.0f/16.0f) );
            glm::vec2 uv_down_left  = glm::vec2( uv_x           , 1.0f - (uv_y + 1.0f/16.0f) );
            UVs.push_back(uv_up_left   );
            UVs.push_back(uv_down_left );
            UVs.push_back(uv_up_right  );

            UVs.push_back(uv_down_right);
            UVs.push_back(uv_up_right);
            UVs.push_back(uv_down_left);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
}
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//Render to display
//-------------------------------------------------------------------------------

void ObjectBillboard::drawObject()
{
	// Bind shader
	glUseProgram(programID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(uniformID, 0);

	// 1st attribute buffer : vertices
	GLuint vertexPS = glGetAttribLocation(programID, const_cast<const char*>("vertexPositionScreen"));
	glEnableVertexAttribArray(vertexPS);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(vertexPS, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : UVs
	GLuint vertexUV = glGetAttribLocation(programID, const_cast<const char*>("vertexUV"));
	glEnableVertexAttribArray(vertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(vertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(vertexUV);
	glDisableVertexAttribArray(vertexPS);



}

