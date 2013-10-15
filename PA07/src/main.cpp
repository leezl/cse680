/*
Project 07, Graphics
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

#include "object.h"

// --Data types (other files now)

// --Evil Global variables
// Just for this example!
int w = 640, h = 480;  // Window size
//GLuint programShading, programTextures;  // The GLSL program handle
Program * programShading=NULL;
Program * programTextures=NULL;
//std::vector<Object* > objects;
Object *whatIsIt=NULL; //stores object. Yay. Gluint, Vertices, Indices, loader
Object *sun=NULL;
float rotationSpeed = 120.0;
bool rotateFlag=false;
float scaler = 1.0;
float dist = -12.0;
float height = 6.0;
bool disableColor=true, disableTextures=false;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
//move model matrix to object? well...
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
Light light;
glm::vec4 lightposition;

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

inline bool file_exists_test (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}


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


    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Project 07");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if ( status != GLEW_OK ) {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    const unsigned char * version ;
    version = (const unsigned char *)glGetString(GL_VERSION);
    printf ("OpenGL version is %s\n", version);

    std::cout<<"Loading "<<filename<<std::endl;
    ilInit();//initialize devil
    iluInit();//more
    //ilutRenderer(ILUT_OPENGL);
    glActiveTexture( GL_TEXTURE0 );//not sure where this needs to be...
    whatIsIt = new Object(path+"/", filename);
    //whatIsIt->flipNormals();
    if(file_exists_test("assets/models/sun.obj")) {
        sun = new Object("assets/models/", "assets/models/sun.obj");
    } else {
        sun = new Object();
    }
    sun->flipNormals();//so it glows with the light inside

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

    //draw object
    whatIsIt->drawObject(); //object: handles program setup
    sun->drawObject(); //object

    // swap the buffers
    glutSwapBuffers();
}

/*
Update: changes postions and updates display
P01: added the rotation about the object's y-axis (after translate)
P02: check a flag for rotation
 */
void update() {
    //update light position
    light.pos = lightposition;
    //std::cout<<"Light in main "<<light.pos.x<<','<<light.pos.y<<','<<light.pos.z<<std::endl;

    static float rotAngle = 0.0;
    float dt = getDT(); 
    if ( rotateFlag ) {
      rotAngle += dt * rotationSpeed;  // this is degrees
    }

    //move object
    whatIsIt->model = glm::scale(glm::mat4(1.0f), glm::vec3(scaler, scaler, scaler));
    whatIsIt->model = glm::rotate(whatIsIt->model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    //move sun 
    sun->model = glm::translate(glm::mat4(1.0f), glm::vec3(light.pos.x, light.pos.y, light.pos.z));
    sun->model = glm::scale(sun->model, glm::vec3(0.15, 0.15, 0.15));

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
    case 83://S
       scaler +=0.1;
       break;
    case 'k'://1
       lightposition.y -= 0.1;
       break;
    case 'i'://2
       lightposition.y +=0.1;
       break;
    case 'j'://1
       lightposition.x -= 0.1;
       break;
    case 'l'://2
       lightposition.x +=0.1;
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

    // Create a Vertex Buffer object to store this vertex info on the GPU
    whatIsIt->initializeObject();
    sun->initializeObject();

    programShading = new Program(true, false, false);//normals, !color, !texture
    programTextures = new Program(true, false, true);//true);//normals, !color, texture (no blending)

    if (whatIsIt->hasTextures && !disableTextures) {
        std::cout<<"using textures"<<std::endl;
        whatIsIt->setProgram(programTextures);
    } else {
        whatIsIt->setProgram(programShading);
        /*std::cout<<"object shader ";
        std::cout<<programShading;
        std::cout<<std::endl;*/
    }
    if (sun->hasTextures && !disableTextures) {
        std::cout<<"using textures for sun"<<std::endl;
        sun->setProgram(programTextures);
    } else {
        sun->setProgram(programShading);
        /*std::cout<<"sun shader ";
        std::cout<<programShading;
        std::cout<<std::endl;*/
    }

    // --Init the view and projection matrices
    //  if you will be having a moving camera
    // the view matrix will need to be more dynamic
    //  ...Like you should update it before you render more dynamic
    //Look at center of object:
    glm::vec3 center  = glm::vec3(whatIsIt->center[0], whatIsIt->center[1], whatIsIt->center[2]);
    glm::vec3 max = glm::vec3(whatIsIt->max[0], whatIsIt->max[1], whatIsIt->max[2]);
    glm::vec3 min = glm::vec3(whatIsIt->min[0], whatIsIt->min[1], whatIsIt->min[2]);
    dist = max[2]+(0.5*(max[2]-min[2]));
    height = max[1]+(0.5*(max[1]-min[1]));
    //ensure light outside of object
    //Add a default light; assume single light for now
    lightposition = glm::vec4(0.0f+(0.5*(max[2]-min[2])), height+(0.5*(max[1]-min[1])), dist, 1.0f);
    light.pos = glm::vec4(0.0f+(0.5*(max[2]-min[2])), height+(0.5*(max[1]-min[1])), dist, 1.0f);
    light.amb = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    light.diff = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    light.spec = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
    //std::cout<<"Light First: "<<light.pos[0]<<' '<<light.pos[1]<<' '<<light.pos[2]<<std::endl;

    //ensure camera outside of object
    view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
                       center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
                       glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

    projection = glm::perspective(45.0f,  // the FoV typically 90 degrees is good which is what this is set to
                                  float(w)/float(h),  // Aspect Ratio, so Circles stay Circular
                                  0.01f,  // Distance to the near plane, normally a small value like this
                                  100.0f);  // Distance to the far plane,

    //update lights in program here, since we move them...
    programShading->addLight(light);
    programTextures->addLight(light);
    programShading->addView(view);
    programTextures->addView(view);
    programShading->addProjection(projection);
    programTextures->addProjection(projection);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );

    // and its done
    return true;
}

void cleanUp() {
    // Clean up, Clean up
    delete programShading;
    delete programTextures;
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
