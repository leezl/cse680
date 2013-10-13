/*
Program cpp
*/
#include "program.h"

// print out matrix by rows
void printMat(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}

Program::Program(bool useNormals/*=false*/, 
				bool useColors/*=false*/, 
				bool useTextures/*=false*/) {
	needsNormals = useNormals;
	needsColors = useColors;
	needsTextures = useTextures;
    light=NULL;
    view=NULL;
    projection=NULL;
	if (compileShaders()) {
		//continue to find locations necessary
        if(loadLocations()) {
        	//success, what else do we need to do?
        } else {
        	std::cerr<<"Failed to locate value in program."<<std::endl;
			exit(EXIT_FAILURE);
        }
	} else {
		//error
		std::cerr<<"Failed to compile shaders for program."<<std::endl;
		exit(EXIT_FAILURE);
	}
}

bool Program::compileShaders() {
	//create our most basic two shaders (always have these)
	Shader *vertex_shader, *fragment_shader; 

    //shaders which needs only normals; no color, no texture, default light or given
	if (needsNormals &&  !needsColors &&  !needsTextures) {// t,f,f
        std::cout<<"Creating shader using vertices and normals only."<<std::endl;
    	vertex_shader = new Shader(GL_VERTEX_SHADER, "assets/shaders/BPVertShader.vs");
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	} else if (!needsNormals && needsColors &&  !needsTextures) { // f,t,f //not implemented yet
        std::cout<<"Creating shader using vertices and colors only."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER, "assets/shaders/colorVertShader.vs");
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	} else if (needsNormals && !needsColors && needsTextures) { // t,f,t //not implemented yet
        std::cout<<"Creating shader using vertices, normals and textures only."<<std::endl;
		std::cerr<<"WARNING: chosen shader type not "
		"implemented, using default type. May see errors"
		" and unwanted behavior."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER);
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	} else if ( needsNormals &&  needsColors &&  !needsTextures) { // t,t,f //not implemented yet
        std::cout<<"Creating shader using vertices, normals, and colors only."<<std::endl;
		std::cerr<<"WARNING: chosen shader type not "
		"implemented, using default type. May see errors"
		" and unwanted behavior."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER);
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
    } else if ( !needsNormals &&  !needsColors &&  !needsTextures) { // f,f,f
        std::cout<<"Creating shader using vertices only."<<std::endl;
        //USES pass through all, no only expects vertices, color set to default
        //set default (vertices only, default colors)
        vertex_shader = new Shader(GL_VERTEX_SHADER);
        fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	} else if ( needsNormals &&  needsColors &&  needsTextures) { // t,t,t
        std::cout<<"Creating shader using vertices, normals, colors, and textures."<<std::endl;
        std::cerr<<"WARNING: chosen shader type not "
        "implemented, using default type. May see errors"
        " and unwanted behavior."<<std::endl;
        //USES pass through all, no only expects vertices, color set to default
        //set default (vertices only, default colors)
        vertex_shader = new Shader(GL_VERTEX_SHADER);
        fragment_shader = new Shader(GL_FRAGMENT_SHADER);
    } else { //default omg shaders
		std::cerr<<"WARNING: chosen shader type not "
		"implemented, using default type. May see errors"
		" and unwanted behavior."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER);
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	}
	//check shader status
	GLint shader_status;
	// Now we link the 2 shader objects into a program
    // This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader->get());
    glAttachShader(program, fragment_shader->get());
    glLinkProgram(program);
    //ehh...might be needed
    delete vertex_shader;
    delete fragment_shader;

    // check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if ( !shader_status ) {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Compilation error in program: %s\n", strInfoLog);
        return false;
    }
    return true;
}

bool Program::loadLocations() {
	// Now we set the locations of the attributes and uniforms
    std::cout<<"Loading Locations"<<std::endl;
    // this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if ( loc_position == -1 ) {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_mmat = glGetUniformLocation(program,
                    const_cast<const char*>("M"));
    if ( loc_mmat == -1 ) {
        std::cerr << "[F] MMATRIX NOT FOUND" << std::endl;
        return false;
    }

    loc_vmat = glGetUniformLocation(program,
                    const_cast<const char*>("V"));
    if ( loc_vmat == -1 ) {
        std::cerr << "[F] VMATRIX NOT FOUND" << std::endl;
        return false;
    }

    loc_pmat = glGetUniformLocation(program,
                    const_cast<const char*>("P"));
    if ( loc_pmat == -1 ) {
        std::cerr << "[F] PMATRIX NOT FOUND" << std::endl;
        return false;
    }

    //the following are optional
    //could get rid of bools and just chekc if -1 is given by getAttr, and use loc as flag...
    //COLORS
    loc_color = glGetAttribLocation(program,
                        const_cast<const char*>("v_color"));
    if ( loc_color == -1 ) {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        if(needsColors) return false;
    }

    //NORMALS
    loc_normal = glGetAttribLocation(program,
                    const_cast<const char*>("v_normal"));
    if ( loc_normal == -1 ) {
        std::cerr << "[F] V_NORMAL NOT FOUND" << std::endl;
        if(needsNormals) return false;
    }

	//TEXTURES
    loc_uv = glGetAttribLocation(program,
                    const_cast<const char*>("v_uv"));
    if ( loc_uv == -1 ) {
        std::cerr << "[F] V_UV NOT FOUND" << std::endl;
        if(needsTextures) return false;
    }

	//load other locations...have defaults if don't exist
	//Light Position
    lightin.loc_LightPos = glGetUniformLocation(program,
                    const_cast<const char*>("LightPosition"));
    if ( lightin.loc_LightPos == -1 ) {
        std::cerr << "LightPosition NOT FOUND" << std::endl;
        return false;
    }

    //Diffuse Product
    lightin.loc_DiffProd = glGetUniformLocation(program,
                    const_cast<const char*>("DiffuseProduct"));
    if ( lightin.loc_DiffProd == -1 ) {
        std::cerr << "DiffueProduct NOT FOUND" << std::endl;
        return false;
    }

    //Specular Product
    lightin.loc_SpecProd = glGetUniformLocation(program,
                    const_cast<const char*>("SpecularProduct"));
    if ( lightin.loc_SpecProd == -1 ) {
        std::cerr << "SpecularProduct NOT FOUND" << std::endl;
        return false;
    }

    //Ambient Product
    lightin.loc_AmbProd = glGetUniformLocation(program,
                    const_cast<const char*>("AmbientProduct"));
    if ( lightin.loc_AmbProd == -1 ) {
        std::cerr << "AmbientProduct NOT FOUND" << std::endl;
        return false;
    }

    //Shininess
    lightin.loc_Shin = glGetUniformLocation(program,
                    const_cast<const char*>("Shininess"));
    if ( lightin.loc_Shin == -1 ) {
        std::cerr << "Shininess NOT FOUND" << std::endl;
        return false;
    }

	return true;
}

//adds in pointers to various scene stuff: lights, view, model, projection...
bool Program::setProgramConstants() {
	//for setting all at once, not here yet :)
    return true;
}

bool Program::setModel(glm::mat4 * modeler) {
	if (modeler!=NULL) {
        //std::cout<<"Setting model "<<std::endl;
        //printMat(*modeler);
		glUniformMatrix4fv(loc_mmat, 1, GL_FALSE, glm::value_ptr(*modeler));//varys across objects
		return true;
	} else {
		//std::cerr<<"No model matrix provided. using ident. "<<std::endl;
		glUniformMatrix4fv(loc_mmat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));//varys across objects
		return true;
	}
}

bool Program::setView(glm::mat4 * viewer) {
    //std::cout<<"View loc "<<view<<std::endl;
	if (viewer!=NULL) {
        //std::cout<<"Setting viewer "<<std::endl;
        //printMat(*viewer);
	    glUniformMatrix4fv(loc_vmat, 1, GL_FALSE, glm::value_ptr(*viewer));//same, usually
	    return true;
	} else {
		//std::cerr<<"No view matrix provided. using current. "<<std::endl;
        //printMat(*view);
		glUniformMatrix4fv(loc_vmat, 1, GL_FALSE, glm::value_ptr(*view));//varys across objects
		return true;
	}
}

bool Program::setProjection(glm::mat4 * projectioner) {
    //std::cout<<"projection loc "<<projection<<std::endl;
	if (projectioner!=NULL) {
        //std::cout<<"Setting projection "<<std::endl;
        //printMat(*projectioner);
	    glUniformMatrix4fv(loc_pmat, 1, GL_FALSE, glm::value_ptr(*projectioner));//same, usually
	    return true;
    } else {
		//std::cerr<<"No projection matrix provided. using current. "<<std::endl;
        //printMat(*projection);
		glUniformMatrix4fv(loc_pmat, 1, GL_FALSE, glm::value_ptr(*projection));//varys across objects
		return true;
	}
}

//vector later
bool Program::setLightPosition(Light * lighter, glm::mat4 * viewer) {
    //std::cout<<"Setting Light "<<std::endl;
    //std::cout<<light<<std::endl;
    //std::cout<<light->pos.x<<" , "<<light->pos.y<<" , "<<light->pos.z<<" , "<<light->pos.w<<std::endl;
    //std::cout<<light->amb.x<<" , "<<light->amb.y<<" , "<<light->amb.z<<" , "<<light->amb.w<<std::endl;
    if (viewer!=NULL) {
        if (lighter!=NULL) {
    	    glUniform4fv(lightin.loc_LightPos, 1, glm::value_ptr((*viewer) * (lighter->pos)));
    	    return true;
        } else {
            glUniform4fv(lightin.loc_LightPos, 1, glm::value_ptr((*viewer) * (light->pos)));
            return true;
        }
    } else {
        if (lighter!=NULL) {
            glUniform4fv(lightin.loc_LightPos, 1, glm::value_ptr((*view) * (lighter->pos)));
            return true;
        } else {
            glUniform4fv(lightin.loc_LightPos, 1, glm::value_ptr((*view) * (light->pos)));
            return true;
        }
    }
}

void Program::addLight(Light & lighter) {
    //std::cout<<"Init Light"<<std::endl;
	//vector later
	light = &lighter;
    //std::cout<<light<<' '<<&lighter<<std::endl;
    //std::cout<<light->pos.x<<" , "<<light->pos.y<<" , "<<light->pos.z<<" , "<<light->pos.w<<std::endl;
    //std::cout<<light->amb.x<<" , "<<light->amb.y<<" , "<<light->amb.z<<" , "<<light->amb.w<<std::endl;
}

void Program::addView(glm::mat4 & viewer){
	//adds view
	view = &viewer;
    //std::cout<<&viewer<<' '<<glm::value_ptr(viewer)<<std::endl;
    //std::cout<<"Init view: "<<view<<' '<<&viewer<<std::endl;
    //printMat(*view);
}

void Program::addProjection(glm::mat4 & projectioner) {
	//adds projection
	projection = &projectioner;
    //std::cout<<"Init projection: "<<projection<<' '<<&projectioner<<std::endl;
    //printMat(*projection);
}

bool Program::startProgram() {
	// enable the shader program
    glUseProgram(program); //can vary across objects

    //send other attributes // done in setBLAH() functions...
    //lights
    //lighting

    //enable attributes
    glEnableVertexAttribArray(loc_position); //same (had better be)
    if (loc_normal!=-1) {
    	glEnableVertexAttribArray(loc_normal);
    }
    if (loc_color!=-1) {
    	glEnableVertexAttribArray(loc_color);
    }
    if (loc_uv!=-1) {
    	glEnableVertexAttribArray(loc_uv);
    }
    //send attributes (handled elsewhere)
    return true;
}

bool Program::stopProgram() {
	// clean up
    glDisableVertexAttribArray(loc_position);
    if (loc_normal!=-1) {
    	glDisableVertexAttribArray(loc_normal);
    }
    if (loc_color!=-1) {
    	glDisableVertexAttribArray(loc_color);
    }
    if (loc_uv!=-1) {
    	glDisableVertexAttribArray(loc_uv);
    }

    glUseProgram(0);
    return true;
}

Program::~Program() {
	glDeleteProgram(program);
}