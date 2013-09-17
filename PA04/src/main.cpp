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
GLuint vbo_geometry, moon_geo;  // VBO handle for our geometry
char *rotDirStr = "Rotating Right";

// Why am I adding more Globals?
bool rotateFlag = true;
float rotationSpeed, rotationSpeedPlanet = 120.0;

// uniform locations
GLint loc_mvpmat;  // Location of the modelviewprojection matrix in the shader

// attribute locations
GLint loc_position;
GLint loc_color;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
glm::mat4 modelP;  // obj->world each object should have its own model matrix
glm::mat4 modelM;  
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
glm::mat4 mvp;  // premultiplied modelviewprojection

// --GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);

// --Resource management
bool initialize();
void cleanUp();
char* loadShader(char* filename);
void createRotationMenu();
void rotation_menu(int id);
void rotation_speed(int key, int x, int y);
void mouse_rotation(int button, int state, int x, int y);

// --Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;


// --Main
// PA2:
//    *added rotation menu creation.
//    *added special keyboard function (arrows)
int main(int argc, char **argv) {
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

    // Create GLUT menu system
    createRotationMenu();

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);  // Called when its time to display
    glutReshapeFunc(reshape);  // Called if the window is resized
    glutIdleFunc(update);  // Called if there is nothing else to do
    glutKeyboardFunc(keyboard);  // Called if there is keyboard input
    glutSpecialFunc(rotation_speed);  // special key function
    glutMouseFunc(mouse_rotation);  // mouse callback

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
    mvp = projection * view * modelP;

    // enable the shader program
    glUseProgram(program);

    // upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    // set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    // set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer(loc_position,  // location of attribute
                          3,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          sizeof(Vertex),  // stride
                          0);  // offset

    glVertexAttribPointer(loc_color,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    glDrawArrays(GL_TRIANGLES, 0, 36);  // mode, starting index, count

    // SECOND OBJECT< NEED STORAGE AND LOOPS LATER
    //calculate other matrix
    mvp = projection * view * modelM;
    // upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //Try drawing moon here...
    glBindBuffer(GL_ARRAY_BUFFER, moon_geo);
    // set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer(loc_position,  // location of attribute
                          3,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          sizeof(Vertex),  // stride
                          0);  // offset

    glVertexAttribPointer(loc_color,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    glDrawArrays(GL_TRIANGLES, 0, 36);  // mode, starting index, count

    // clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
    glUseProgram(0);
    int lenghOfString = (int)strlen(rotDirStr);
    glRasterPos2f(0.0-0.9, 0.0+0.9);
    for ( int i = 0; i < lenghOfString; i++ ) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, rotDirStr[i]);
    }

    // swap the buffers
    glutSwapBuffers();
}

/*
Update: changes postions and updates display
P01: added the rotation about the object's y-axis (after translate)
P02: check a flag for rotation
 */
void update() {
    // total time
    static float angle = 0.0;
    static float angleMoon = 0.0;
    static float rotAngle = 0.0;
    static float rotAngleMoon = 0.0;
    float dt = getDT();  // if you have anything moving, use dt.

    if ( rotationSpeedPlanet >0.0 ) {
        rotDirStr = "Rotating Right";
    }
    else {
        rotDirStr = "Rotating Left";
    }

    angle += dt * M_PI/2;  // move through 90 degrees a second
    angleMoon += dt * M_PI;
    if ( rotateFlag ) {
      rotAngle += dt * rotationSpeedPlanet;  // this is degrees
    }
    rotAngleMoon += dt * rotationSpeed;

    modelP = glm::translate(glm::mat4(1.0f),
                            glm::vec3(4.25 * sin(angle),
                                      0.0, 4.25 * cos(angle)));
    // The following should: rotate about z axis
    // Where params are: (initial matrix, angle, axis)
    modelP = glm::rotate(modelP, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    modelP = glm::scale(modelP, glm::vec3(0.75f, 0.75f, 0.75f));

    //move moon too
    modelM = glm::translate(glm::mat4(1.0f),
                            glm::vec3(4.25 * sin(angle),
                                      0.0, 4.25 * cos(angle)));
    modelM = glm::translate(modelM,
                            glm::vec3(2.0 * sin(angleMoon),
				      0.0, 2.0 * cos(angleMoon)));
    modelM = glm::rotate(modelM, rotAngleMoon, glm::vec3(0.0f, 1.0f, 0.0f));
    modelM = glm::scale(modelM, glm::vec3(0.4f, 0.4f, 0.4f));

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
      rotationSpeedPlanet -= 10.0;
      break;
    case 100:  // d
      rotationSpeedPlanet += 10.0;
      break;
    case 32:  // space
    case 115:  // s
      rotationSpeedPlanet = -rotationSpeedPlanet;
    default:
      break;
    }
}

bool initialize() {
    // Initialize basic geometry and shaders for this example

    // this defines a cube, this is why a model loader is nice
    // you can also do this with a draw elements
    // and indices, try to get that working
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };
    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);
    // Can I reuse these buffers? they're both cubes...
    glGenBuffers(1, &moon_geo);
    glBindBuffer(GL_ARRAY_BUFFER, moon_geo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

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
    view = glm::lookAt(glm::vec3(0.0, 8.0, -16.0),  // Eye Position
                       glm::vec3(0.0, 0.0, 0.0),  // Focus point
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
    glDeleteBuffers(1, &vbo_geometry);
}

// returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

/*
PA02
input: none
output: none
process: Creates a menu for rotation and closing the program.
*/
void createRotationMenu() {
  glutCreateMenu(rotation_menu);
  glutAddMenuEntry("start/stop rotation", 1);
  glutAddMenuEntry("quit", 2);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  return;
}

 /*
PA02
Input: menu item id
Output: none
Process: start or stop rotation, or quit
  */
void rotation_menu(int id) {
  switch (id) {
  case 1:
      rotateFlag = !rotateFlag;
      break;
  case 2:
      exit(0);
      break;
  default:
    std::cerr << "Rotation menu sent something weird..." << id << std::endl;
  }
  glutPostRedisplay();
  return;
}

/*
PA02
Input: key pressed, mouse location
Output: none
Process: change rotation speed if up or down keys were pressed.
*/
void rotation_speed(int key, int x, int y) {
  // x and y are mouse position: how can we use this?
  switch (key) {
    case GLUT_KEY_UP:
      rotationSpeedPlanet += 10.0;
      // rotationSpeed = fmod((rotationSpeed+10.0), 360.0);
      break;
    case GLUT_KEY_DOWN:
      rotationSpeedPlanet -= 10.0;
      // rotationSpeed = fmod((rotationSpeed-10.0), 360.0);
      break;
    case GLUT_KEY_LEFT:
      if ( rotationSpeedPlanet>0 ) {
          rotationSpeedPlanet = -rotationSpeedPlanet;
      }
      break;
    case GLUT_KEY_RIGHT:
      if ( rotationSpeedPlanet<0 ) {
          rotationSpeedPlanet = -rotationSpeedPlanet;
      }
      break;
    }
  glutPostRedisplay();
  return;
}
/*
Input: button id, state of button, position of mouse
Output: none
Process: reverse direction on left click
 */
void mouse_rotation(int button, int state, int x, int y) {
  if  (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    rotationSpeedPlanet = -rotationSpeedPlanet;
  return;
}
