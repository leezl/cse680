/*
Project 04, Graphics
Edited: Liesl Wigand
Copyright 2013 Liesl Wigand

 */


#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting
#include <chrono>
#include <typeinfo>  // debugger
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  // Makes passing matrices to shaders easier

#include "shader.h"
#include "object.h"

// --Data types
// This object will define the attributes of a vertex(position, color, etc...)
struct Vertex {
    GLfloat position[3];
    GLfloat color[3];
};

// --Evil Global variables
// Just for this example!
int w = 640, h = 480;  // Window size
GLuint program;  // The GLSL program handle
Object *whatIsIt; //stores object. Yay. Gluint, Vertices, Indices, loader
GLuint vbo_geometry;
float rotationSpeed = 120.0;
bool rotateFlag=true;
float scaler = 1.0;
float dist = -12.0;
float height = 6.0;

// uniform locations
GLint loc_mvpmat;  // Location of the modelviewprojection matrix in the shader

// attribute locations
GLint loc_position;
//GLint loc_normal;
//GLint loc_uv;
GLint loc_color;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
//move model matrix to object? well...
glm::mat4 model;  // obj->world each object should have its own model matrix 
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
glm::mat4 mvp;  // premultiplied modelviewprojection

// --GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void arrow_keys(int key, int x, int y);

// --Resource management
bool initialize();
void cleanUp();

// --Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;


// --Main
// PA2:
//    *added rotation menu creation.
//    *added special keyboard function (arrows)
int main(int argc, char **argv) {
    //need filename for object file...
    std::string filename;
    std::string path;
    if (argc<2) {
        std::cerr<<"Wrong number of arguments. Backup Plan: cin. "<<std::endl;
        std::cout<<"What's the name of the obj file? (include path and ext...) > ";
        std::cin>>filename;
    } else {
        filename = argv[1];
    }

    //my default path and ext adder
    //filename.insert(0, "assets/models/");
    //filename.append(".obj");
    //parse into file and path; assume path is also used for mtl
    std::size_t found = filename.find("/");
    std::size_t mid = 0;
    while ( found != std::string::npos) {
        mid = found; //grab old end of path
        found  = filename.find("/", mid+1); //find next directory
    }
    path = filename.substr(0, mid);
    std::cout<<"Path: "<<path<<std::endl;

    std::cout<<"Loading "<<filename<<std::endl;
    whatIsIt = new Object(path+"/", filename);

    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Project 02");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if ( status != GLEW_OK ) {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }


    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);  // Called when its time to display
    glutReshapeFunc(reshape);  // Called if the window is resized
    glutIdleFunc(update);  // Called if there is nothing else to do
    glutKeyboardFunc(keyboard);  // Called if there is keyboard input
    glutSpecialFunc(arrow_keys);

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if ( init ) {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

// --Implementations
void render() {
    // --Render the scene

    // clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // premultiply the matrix for this example
    mvp = projection * view * model;

    // enable the shader program
    glUseProgram(program);

    // upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //enable attributes
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);  

    //draw object
    whatIsIt->drawObject(loc_position, 0, 0, loc_color);

    // clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);

    glUseProgram(0);

    // swap the buffers
    glutSwapBuffers();
}

/*
Update: changes postions and updates display
P01: added the rotation about the object's y-axis (after translate)
P02: check a flag for rotation
 */
void update() {
    static float rotAngle = 0.0;
    float dt = getDT(); 
    if ( rotateFlag ) {
      rotAngle += dt * rotationSpeed;  // this is degrees
    }

    //move object
    model = glm::scale(glm::mat4(1.0f), glm::vec3(scaler, scaler, scaler));
    model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    //move view...
    glm::vec3 center  = glm::vec3(whatIsIt->center[0], whatIsIt->center[1], whatIsIt->center[2]);
    view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
                       center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
                       glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

    // Update the state of the scene
    glutPostRedisplay();  // call the display callback
}


void reshape(int n_w, int n_h) {
    w = n_w;
    h = n_h;
    // Change the viewport to be correct
    glViewport(0, 0, w, h);
    // Update the projection matrix as well
    // See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);
}

void keyboard(unsigned char key, int x_pos, int y_pos) {
    // Handle keyboard input
  switch (key) {
    case 27:  // ESC
      exit(0);
      break;  // why is this necessary...
    case 97:  // a
      rotationSpeed -= 10.0;
      break;
    case 100:  // d
      rotationSpeed += 10.0;
      break;
    case 32:
      rotateFlag = !rotateFlag;
      break;
    case 115://s
       scaler -= 0.1;
       break;
    case 83:
       scaler +=0.1;
       break;
    default:
      break;
    }
}

void arrow_keys(int key, int x, int y) {
  // x and y are mouse position: how can we use this?
  switch (key) {
    case GLUT_KEY_UP:
      height += 1.0;
      break;
    case GLUT_KEY_DOWN:
      height -= 1.0;
      break;
    case GLUT_KEY_LEFT:
      dist += 1.0;
      break;
    case GLUT_KEY_RIGHT:
      dist -= 1.0;
      break;
    }
  glutPostRedisplay();
  return;
}

bool initialize() {
    // Initialize basic geometry and shaders for this example

    // this defines a cube, this is why a model loader is nice
    // you can also do this with a draw elements
    // and indices, try to get that working

    // Create a Vertex Buffer object to store this vertex info on the GPU
    whatIsIt->initializeObject();

    // --Geometry done

    Shader vertex_shader(GL_VERTEX_SHADER);
    Shader fragment_shader(GL_FRAGMENT_SHADER);

    GLint shader_status;

    // Now we link the 2 shader objects into a program
    // This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader.get());
    glAttachShader(program, fragment_shader.get());
    glLinkProgram(program);
    // check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if ( !shader_status ) {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    // Now we set the locations of the attributes and uniforms
    // this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if ( loc_position == -1 ) {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if ( loc_color == -1 ) {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if ( loc_mvpmat == -1 ) {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }

    // --Init the view and projection matrices
    //  if you will be having a moving camera
    // the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic
    //  for this project having them static will be fine
    //Look at center of object:
    glm::vec3 center  = glm::vec3(whatIsIt->center[0], whatIsIt->center[1], whatIsIt->center[2]);
    glm::vec3 max = glm::vec3(whatIsIt->max[0], whatIsIt->max[1], whatIsIt->max[2]);
    dist = max[2]+1.0;
    height = max[1]+1.0;
    view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
                       center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
                       glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

    projection = glm::perspective(45.0f,  // the FoV typically 90 degrees is good which is what this is set to
                                  float(w)/float(h),  // Aspect Ratio, so Circles stay Circular
                                  0.01f,  // Distance to the near plane, normally a small value like this
                                  100.0f);  // Distance to the far plane,

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // and its done
    return true;
}

void cleanUp() {
    // Clean up, Clean up
    glDeleteProgram(program);
    //whatIsIt->cleanUp();
}

// returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
