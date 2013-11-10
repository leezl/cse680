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

std::string convertInt(int number) {
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

Program::Program(bool useNormals/*=false*/, 
				bool useColors/*=false*/, 
				bool useTextures/*=false*/) {
	needsNormals = useNormals;
	needsColors = useColors;
	needsTextures = useTextures;
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
    	vertex_shader = new Shader(GL_VERTEX_SHADER, "assets/shaders/s2140VertShader.vs");
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER, "assets/shaders/s2140FragShader.fs");
	} else if (!needsNormals && needsColors &&  !needsTextures) { // f,t,f //not implemented yet
        std::cout<<"Creating shader using vertices and colors only."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER, "assets/shaders/c140VertShader.vs");
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER);
	} else if (needsNormals && !needsColors && needsTextures) { // t,f,t
        std::cout<<"Creating shader using vertices, normals and textures only."<<std::endl;
		//set default (vertices only, default colors)
		vertex_shader = new Shader(GL_VERTEX_SHADER, "assets/shaders/ts2140VertShader.vs");
    	fragment_shader = new Shader(GL_FRAGMENT_SHADER, "assets/shaders/ts2140FragShader.fs");
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

    loc_timat = glGetUniformLocation(program,
                    const_cast<const char*>("TIMAT"));
    if (loc_timat == -1) {
        std::cerr << "[F] Trans Inv Matrix NOT FOUND " << std::endl;
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
    //texture image loc
    loc_tex = glGetUniformLocation(program,
                    "textSamp");
    if ( loc_tex == -1 ) {
        std::cerr<< "[F] textSamp NOT FOUND" << std::endl;
        if (needsTextures) return false;
    }

	//load other locations...have defaults if don't exist
    for(int i=0; i<8; i++) {
        //Light Position
        std::cout<<"lights["<<convertInt(i)<<"].position "<<std::endl;
        lightPos[i].locs[0] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].position").c_str()));
        if ( lightPos[i].locs[0] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].position NOT FOUND" << std::endl;
            return false;
        }
        //Diffuse Light
        lightPos[i].locs[1] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].diffuse").c_str()));
        if ( lightPos[i].locs[1] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].diffuse NOT FOUND" << std::endl;
            return false;
        }
        //specular Light
        lightPos[i].locs[2] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].specular").c_str()));
        if ( lightPos[i].locs[2] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].specular NOT FOUND" << std::endl;
            return false;
        }
        //ambient light
        lightPos[i].locs[3] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].ambient").c_str()));
        if ( lightPos[i].locs[3] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].ambient NOT FOUND" << std::endl;
            return false;
        }
        //constantAtten
        lightPos[i].locs[4] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].constantAttenuation").c_str()));
        if ( lightPos[i].locs[4] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].constantAttenuation NOT FOUND" << std::endl;
            return false;
        }
        //linearAtten
        lightPos[i].locs[5] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].linearAttenuation").c_str()));
        if ( lightPos[i].locs[5] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].linearAttenuation NOT FOUND" << std::endl;
            return false;
        }
        //quadraticAtten
        lightPos[i].locs[6] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].quadraticAttenuation").c_str()));
        if ( lightPos[i].locs[6] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].quadraticAttenuation NOT FOUND" << std::endl;
            return false;
        }
        //spotCutoff
        lightPos[i].locs[7] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].spotCutoff").c_str()));
        if ( lightPos[i].locs[7] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].spotCutoff NOT FOUND" << std::endl;
            return false;
        }
        //spotExp
        lightPos[i].locs[8] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].spotExponent").c_str()));
        if ( lightPos[i].locs[8] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].spotExponent NOT FOUND" << std::endl;
            return false;
        }
        //spotDir
        lightPos[i].locs[9] = glGetUniformLocation(program,
                    const_cast<const char*>(("lights["+convertInt(i)+"].spotDirection").c_str()));
        if ( lightPos[i].locs[9] == -1 ) {
            std::cerr << "lights["+convertInt(i)+"].spotDirection NOT FOUND" << std::endl;
            return false;
        }
    }

    //Material Positions
    materialPosition.locs[0] = glGetUniformLocation(program,
                const_cast<const char*>("material.ambient"));
    if ( materialPosition.locs[0] == -1 ) {
        std::cerr << "material.ambient NOT FOUND" << std::endl;
        return false;
    }
    materialPosition.locs[1] = glGetUniformLocation(program,
                const_cast<const char*>("material.diffuse"));
    if ( materialPosition.locs[1] == -1 ) {
        std::cerr << "material.diffuse NOT FOUND" << std::endl;
        return false;
    }
    materialPosition.locs[2] = glGetUniformLocation(program,
                const_cast<const char*>("material.specular"));
    if ( materialPosition.locs[2] == -1 ) {
        std::cerr << "material.specular NOT FOUND" << std::endl;
        return false;
    }
    materialPosition.locs[3] = glGetUniformLocation(program,
                const_cast<const char*>("material.shininess"));
    if ( materialPosition.locs[3] == -1 ) {
        std::cerr << "material.shininess NOT FOUND" << std::endl;
        return false;
    }

	return true;
}

//adds.n pointers to various scene stuff: lights, view, model, projection...
bool Program::setProgramConstants() {
	//for setting all at once, not here yet :)
    return true;
}

bool Program::setModelView(glm::mat4 * modeler, glm::mat4 * viewer) {
    setModel(modeler);
    setView(viewer);
    
    if (modeler!=NULL and viewer!=NULL) {
        //std::cout<<"Setting model "<<std::endl;
        //printMat(*modeler)
        glUniformMatrix4fv(loc_timat, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse((*viewer)*(*modeler)))));
        return true;
    } else if (modeler!=NULL) {
        glUniformMatrix4fv(loc_timat, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse((*view)*(*modeler)))));//varys across objects
        return true;
    } else if (viewer!=NULL) {
        glUniformMatrix4fv(loc_timat, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse((*viewer)*glm::mat4(1.0)))));//varys across objects
        return true;
    } else {
        //std::cerr<<"No model matrix provided. using ident. "<<std::endl;
        glUniformMatrix4fv(loc_timat, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse((*view)*glm::mat4(1.0)))));//varys across objects
        return true;
    }
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

bool Program::setTextureSampler(GLuint * texI) {

    if (texI != NULL) {
        //std::cout<<"not null "<<*texI<<std::endl;
        glUniform1i(*texI, 0);
        return true;
    } else {
        //std::cout<<"program setting texture uniform sampler "<<loc_tex<<std::endl;
        //hope we never make it here
        glUniform1i(loc_tex, 0);
        return true;
    }
}

bool Program::setLights() {
    for (int i=0; i<8; i++) {
        //std::cout<<"Setting "<<i<<std::endl;
        if (lights[i]->on) {
            // load position
            glUniform4fv(lightPos[i].locs[0], 1, glm::value_ptr((*view) * (lights[i]->pos)));

            // load ambient
            glUniform4fv(lightPos[i].locs[3], 1, glm::value_ptr(lights[i]->amb));

            // load diffuse
            glUniform4fv(lightPos[i].locs[1], 1, glm::value_ptr(lights[i]->diff));

            // load specular
            glUniform4fv(lightPos[i].locs[2], 1, glm::value_ptr(lights[i]->spec));

            // load spot direction
            glUniform4fv(lightPos[i].locs[9], 1, glm::value_ptr((*view) * (lights[i]->spotDir)));

            // load constant attenuation
            glUniform1f(lightPos[i].locs[4], lights[i]->constantAtten);

            // load linear attenuation
            glUniform1f(lightPos[i].locs[5], lights[i]->linearAtten);

            // load quadratic attenuation
            glUniform1f(lightPos[i].locs[6], lights[i]->quadraticAtten);

            // load spot cutoff
            glUniform1f(lightPos[i].locs[7], lights[i]->spotCutoff);

            // load spot exponent
            glUniform1f(lightPos[i].locs[8], lights[i]->spotExp);
        } else {
            //load off light
            //std::cout<<"offLight: "<<offLight.amb.x<<','<<offLight.amb.y<<','<<offLight.amb.z<<std::endl;
            // load position
            glUniform4fv(lightPos[i].locs[0], 1, glm::value_ptr((*view) * (offLight.pos)));

            // load ambient
            glUniform4fv(lightPos[i].locs[3], 1, glm::value_ptr(offLight.amb));

            // load diffuse
            glUniform4fv(lightPos[i].locs[1], 1, glm::value_ptr(offLight.diff));

            // load specular
            glUniform4fv(lightPos[i].locs[2], 1, glm::value_ptr(offLight.spec));

            // load spot direction
            glUniform4fv(lightPos[i].locs[9], 1, glm::value_ptr((*view) * (offLight.spotDir)));

            // load constant attenuation
            glUniform1f(lightPos[i].locs[4], offLight.constantAtten);

            // load linear attenuation
            glUniform1f(lightPos[i].locs[5], offLight.linearAtten);

            // load quadratic attenuation
            glUniform1f(lightPos[i].locs[6], offLight.quadraticAtten);

            // load spot cutoff
            glUniform1f(lightPos[i].locs[7], offLight.spotCutoff);

            // load spot exponent
            glUniform1f(lightPos[i].locs[8], offLight.spotExp);
        }
    }
}

void Program::addLight(Light & lighter, int index) {
    //std::cout<<"Init Light"<<std::endl;
	//vector later
    lights[index] = &lighter;
    /*lights[index]->pos = lighter.pos;
    lights[index]->amb = lighter.amb;
    lights[index]->diff = lighter.diff;
    lights[index]->spec = lighter.spec;
    lights[index]->constantAtten = lighter.constantAtten;
    lights[index]->linearAtten = lighter.linearAtten;
    lights[index]->quadraticAtten = lighter.quadraticAtten;
    lights[index]->spotCutoff = lighter.spotCutoff;
    lights[index]->spotExp = lighter.spotExp;
    lights[index]->spotDir = lighter.spotDir;*/
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
        glEnable(GL_TEXTURE_2D);
        //std::cout<<"enabling uv"<<std::endl;
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
        glDisable(GL_TEXTURE_2D);
    }

    glUseProgram(0);
    return true;
}

Program::~Program() {
	glDeleteProgram(program);
}