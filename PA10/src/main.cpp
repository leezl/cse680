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
#include "objectBillboard.h"	//Score Display
#include "AI.h"					//AI

// --Data types (other files now)

// --Evil Global variables
// Just for this example!
int w = 640, h = 480;  // Window size
glm::vec3 maxPos = glm::vec3(30.0,30.0,30.0);
glm::vec3 minPos = glm::vec3(-30.0,-30.0,-30.0);
//int w = 1280, h = 960;
//GLuint programShading, programTextures;  // The GLSL program handle
Program * programShading=NULL;
Program * programTextures=NULL;
//std::vector<Object* > objects;
//Object *whatIsIt=NULL; //stores object. Yay. Gluint, Vertices, Indices, loader
Object *board=NULL;
Object *sphere=NULL;
//should be a better way of swapping these out, but kinda short on time
Object *paddle1=NULL;
Object *sun=NULL;
PhysicsWorld* worlds[2];
//wasd, ijkl
//int keyBuffer[8] = {0};//not pressed //only controls
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)

//----------------------------------------------------------------
//New Global Variables - Added for Proj09
//----------------------------------------------------------------
ObjectBillboard *scoreText=NULL;		//Display Score - New - Added for Proj09
AI *ai = NULL;
//----------------------------------------------------------------
//PhysicsWorld* dynamicWorld; //tracks all objects
//PhysicsWorld* staticWorld; //tracks static objects as dynamic (for collisions with each other)
glm::vec3 paddle1Trans = glm::vec3(0.0,0.0,0.0);

//----------------------------------------------------------------
//New Global Variables - Added for Proj09
//----------------------------------------------------------------
int previousMouseX = (int) w/2;	//Mouse Control Storage
int previousMouseY = (int) h/2;	//Mouse Control Storage
int pointsPlayer1 = 0;			//Players points
int pointsPlayer2 = 0;


std::string gameMode = "Defensive";
int playerMode = 1;//ai = 1; 2 player =2?

int statusCounter = 0;
bool gameStatusUpdate = false;
int paddle1X;
int paddle1Y;
int paddle1Z;
bool pauseFlag = false;


//----------------------------------------------------------------
float dist = 25; //changed from -12
float height = 15; //Changed from 6
bool disableColor=true, disableTextures=false, aiEnabled=false;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
//move model matrix to object? well...
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
std::vector < Light > lights;
int activeLight = 0; //use this to switch which light is controled by wasd...

void createMenu();
// --GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyDown(unsigned char key, int x_pos, int y_pos);
void keyUp(unsigned char key, int x_pos, int y_pos);
void keyOperations(void);
void arrow_keys(int key, int x, int y);
//----------------------------------------------------------------
//New GLUT Callback Function - Added for Proj09
//----------------------------------------------------------------
void mouseMove(int x, int y);	//Mouse Cursor Control
void createMenu();
void menuSub(int id);
void menu(int id);			//Menu 
void addLight();
void switchActiveLight();
//void displayStatus(float x, float y, float z, char *string);	//Status 
//---------------------------------------------------------------

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
    std::string path="assets/models";

    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Project 10: Lighting");
    //full screen
    glutFullScreen();

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
    board = new Object(path+"/", "assets/models/hockeyBoardStar.obj");
    paddle1 = new Object(path+"/", "assets/models/paddle2Blue.obj");
    sphere = new Object(path+"/", "assets/models/sphere.obj");

    if(file_exists_test("assets/models/sun.obj")) {
        sun = new Object("assets/models/", "assets/models/sun.obj");
    } else if (file_exists_test("assets/models/sphere.obj")) {
        sun = new Object("assets/models/", "assets/models/sphere.obj");
    } else {
        sun = new Object("defaultSunObj");
    }
    sun->flipNormals();//so it glows with the light inside
	//----------------------------------------------------------------
	//New Objects - Added for Proj09
	//----------------------------------------------------------------
	scoreText = new ObjectBillboard("assets/scoreboard/characters.tga");		//Score display as 2D
	ai = new AI();
	//----------------------------------------------------------------

    //init keyArray
    for (int i =0; i<256; i++) {
        keyStates[i] = false;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);  // Called when its time to display
    glutReshapeFunc(reshape);  // Called if the window is resized
    glutIdleFunc(update);  // Called if there is nothing else to do
    //glutKeyboardFunc(keyboard);  // Called if there is keyboard input
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(arrow_keys);//should move these to another function?
	//----------------------------------------------------------------
	//New GLUT Function - Added for Proj09
	//----------------------------------------------------------------
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)/2 );		//Place cursor close to initial position of paddle 1
	//glutSetCursor(GLUT_CURSOR_NONE);
	//glutMotionFunc(mouseMove);			//Mouse Cursor Position
	//glutPassiveMotionFunc(mouseMove);	//Removes the left button to move paddle
    createMenu();
	//----------------------------------------------------------------

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

void createMenu() {
    int menu;
    menu = glutCreateMenu(menuSub);               //Menu
    glutAddMenuEntry("Restart",2);//Reposition, and blank score
    glutAddMenuEntry("Pause/Resume",1);//combined pause and unpause
    glutAddMenuEntry("quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);  //Right Button Menu
}

// --Implementations
void render() {
    // --Render the scene

    // clear the screen
    glClearColor(0.2, 0.2, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw object
    board->drawObject();
    sphere->drawObject();

    //sun->drawObject();
	paddle1->drawObject();

    //draw other lights: set sun position: drawObject, repeat...
    for (int i =0; i<8; i++) {
        //std::cout<<"Drawing  "<<i<<' '<<lights[i].type<<" what?"<<std::endl;
        if (lights[i].inactive == false && lights[i].type!="ambient") {
            sun->model = glm::translate(glm::mat4(1.0f), glm::vec3(lights[i].pos.x, lights[i].pos.y, lights[i].pos.z));
            sun->model = glm::scale(sun->model, glm::vec3(0.60, 0.60, 0.60));
            sun->drawObject();
        }
    }

    //draw some light parameters on the screen
    //print spotlight direction
    std::stringstream ss;//create a stringstream
    ss << activeLight;
    ss << ' ';
    ss << lights[activeLight].type;
    //std::cout<<GLUT_WINDOW_HEIGHT<<','<<GLUT_WINDOW_WIDTH<<' '<<w<<','<<h<<std::endl;
    const char *activeLights = ("Active Light: "+ss.str()).c_str();
    int lenghOfString = (int)strlen(activeLights);
    glRasterPos2f(0.0-0.98, 0.0+0.98);//10.0);
    for ( int i = 0; i < lenghOfString; i++ ) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, activeLights[i]);
    }
    //print spotlight direction
    if (lights[activeLight].type=="spot") {
        ss << lights[2].spotDir.x;
        ss << ',';
        ss << lights[2].spotDir.y;
        ss << ',';
        ss << lights[2].spotDir.z;
        //std::cout<<GLUT_WINDOW_HEIGHT<<','<<GLUT_WINDOW_WIDTH<<' '<<w<<','<<h<<std::endl;
        const char *spotDirection = ("SpotDirection: "+ss.str()).c_str();
        lenghOfString = (int)strlen(spotDirection);
        glRasterPos2f(0.0-90.0, 0.0+90.0);
        for ( int i = 0; i < lenghOfString; i++ ) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, spotDirection[i]);
        }
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
    //std::cout<<"Light in main "<<light.pos.x<<','<<light.pos.y<<','<<light.pos.z<<std::endl;
    float dt = getDT(); 
    keyOperations();

	if(!pauseFlag)	//Menu Pause and Resume Option 
	{
        //std::cout<<dt<<','<<std::endl;
        worlds[0]->stepWorld(dt, 1);
        worlds[1]->stepWorld(dt, 1);

        board->updateModel();
        sphere->updateModel();

        paddle1->nextTranslate = paddle1Trans; //update user input
        paddle1->updateModel();
        paddle1Trans = paddle1->nextTranslate;

        //add cone for spot light?

		//move view...NEED LIMITS HERE
		glm::vec3 center  = glm::vec3(board->getCurrentCenter()[0], board->getCurrentCenter()[1], board->getCurrentCenter()[2]);
		view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
		                   center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
		                   glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

	}

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

//mark key down
void keyDown (unsigned char key, int x, int y) {
    keyStates[key] = true; // Set the state of the current key to pressed
}
//mark key up
void keyUp (unsigned char key, int x, int y) {
    keyStates[key] = false; // Set the state of the current key to not pressed
}

void switchActiveLight() {
    std::cout<<"Active Light: "<<activeLight<<std::endl;
    activeLight++;
    if (activeLight<8) {
        if (lights[activeLight].inactive == true) {
            activeLight = 0;
        }
    } else {
        activeLight=0;
    }
}

void addLight() {
    int i = 4;
    bool notExit = true;
    while (i<8 && notExit) {
        if (lights[i].inactive == true) {
            lights[i].type = "spot";
            lights[i].inactive = false;
            lights[i].on = true;
            lights[i].pos = glm::vec4(0.0, 2.0, 0.0, 1.0f); //note 1.0 makes directional
            lights[i].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            lights[i].diff = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
            lights[i].spec = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
            lights[i].spotDir = glm::vec4(glm::vec4(0.0,0.0,0.0,1.0)-lights[i].pos);
            lights[i].constantAtten = 1.0;
            lights[i].linearAtten = 1.0;
            lights[i].quadraticAtten = 0.0;
            lights[i].spotCutoff = 45.0;
            lights[i].spotExp = 5.0;
            activeLight = i;
            notExit = false;
        }
        i++;
    }
}
//act on key up or down
//should we call this in render or update? tutorial had it in render...
void keyOperations (void) {
    if (keyStates[27]) {
        // ESC
        exit(0);
    }
    if (keyStates['+']) {
        //add a spot light until 8 full lights
        addLight();
        keyStates['+']=false;
    }
    //swap Active Light
    if (keyStates[' ']) {
        switchActiveLight();
        keyStates[' '] = false;
    }
    //Light controls
    if (keyStates['w']) {
       lights[activeLight].pos.z += 0.1;
    }
    if (keyStates['s']) {
        lights[activeLight].pos.z -= 0.1;
    }
    if (keyStates['a']) {
        lights[activeLight].pos.x -= 0.1;
    }
    if (keyStates['d']) {
        lights[activeLight].pos.x += 0.1;
    }
    if (keyStates['W']) {
        lights[activeLight].pos.y += 0.1;
    }
    if (keyStates['S']) {
        lights[activeLight].pos.y -=0.1;
    }
    if (keyStates['A']) {
        if (lights[activeLight].type=="spot" && lights[activeLight].spotCutoff>0) {
            lights[activeLight].spotCutoff -= 5.0;
        }
        keyStates['A'] = false;
    }
    if (keyStates['D']) {
        if (lights[activeLight].type=="spot" && lights[activeLight].spotCutoff<90) {//chekc this limit?
            lights[activeLight].spotCutoff += 5.0;
        }
        keyStates['D'] = false;
    }
    if (keyStates['q']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotDir.x -= 0.5;
        }
        keyStates['q'] = false;
    }
    if (keyStates['e']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotDir.x += 0.5;
        }
        keyStates['e'] = false;
    }
    if (keyStates['z']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotDir.z += 0.5;
        }
        keyStates['z'] = false;
    }
    if (keyStates['c']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotDir.z -= 0.5;
        }
        keyStates['c'] = false;
    }
    if (keyStates['x']) {
        if (lights[activeLight].type=="spot") {
            //this needs to change
            lights[activeLight].spotDir.y = -lights[activeLight].spotDir.y;
        }
        keyStates['x'] = false;
    }
    if (keyStates['Q']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotExp -= 1.0;
        }
        keyStates['Q'] = false;
    }
    if (keyStates['E']) {
        if (lights[activeLight].type=="spot") {
            lights[activeLight].spotExp += 1.0;
        }
        keyStates['E'] = false;
    }
    //paddle1
    if (keyStates['k']) {//1
       paddle1Trans.z += 0.1;
    }
    if (keyStates['i']) {//2
        paddle1Trans.z -= 0.1;
    }
    if (keyStates['j']) {//1
        paddle1Trans.x -= 0.1;
    }
    if (keyStates['l']) {//2
        paddle1Trans.x += 0.1;
    }
    //Lights on or off
    if (keyStates['1']) {//2
        //point
        lights[0].on = !lights[0].on;
        keyStates['1'] = false;
    }
    if (keyStates['2']) {//2
        //ambient
        lights[1].on = !lights[1].on;
        keyStates['2'] = false;
    }
    if (keyStates['3']) {//2
        //spot
        lights[2].on = !lights[2].on;
        keyStates['3'] = false;
    }
    if (keyStates['4']) {//2
        //distant
        lights[3].on = !lights[3].on;
        keyStates['4'] = false;
    }
}

void arrow_keys(int key, int x, int y) {
  // x and y are mouse position: how can we use this?
  switch (key) {
    case GLUT_KEY_UP:
      if (height<30){
        height += 1.0;
      }
      break;
    case GLUT_KEY_DOWN:
      if (height>-1){
        height -= 1.0;
      }
      break;
    case GLUT_KEY_LEFT:
      if (dist<30) {
        dist += 1.0;
      }
      break;
    case GLUT_KEY_RIGHT:
      if (dist>1) {
        dist -= 1.0;
      }
      break;
    }
  //glutPostRedisplay();
  return;
}

//Mouse Motion Callback Function
// Updates cubeTrans based on position of mouse motion.  Uses 2 global variables - previousMouseX, previousMouseY
void mouseMove(int x, int y) {

	GLfloat depth;
	glReadPixels(x, h-y-25, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	glm::vec4 viewport = glm::vec4(0, 0, w,h);
	glm::vec3 wincoord = glm::vec3(x, h - y+50 , depth);

	glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);

	paddle1Trans = objcoord;

	glutPostRedisplay();
}

void menuSub(int id) {
    switch(id)
    {
        //Pause or unpause
        case 1:
            //pauseFlag = -pauseFlag;
			pauseFlag = pauseFlag == true? false:true;
            break;
        //2 Players or 1 Player
        case 3:
            playerMode = 2;
            gameStatusUpdate = true;
            break;
        //Defensive AI
        case 4:
            gameMode = "Defensive";
            playerMode = 1;
            gameStatusUpdate = true;
            break;      
        //Offensive AI
        case 5:
            gameMode = "Offensive";
            playerMode = 1;
            gameStatusUpdate = true;
            break;      
        //Balanced AI
        case 6:
            gameMode = "Balanced";
            playerMode = 1;
            gameStatusUpdate = true;
            break;  
        case 7:
            break;
        case 8:
            break;
        //Restart
        case 9:
            break;
        //Exit
        case 0:
            exit(0);
            break;
    }

    glutPostRedisplay();
}
//----------------------------------------------------------------

bool initialize() {
    // Initialize basic geometry and shaders for this example
    worlds[0] = new PhysicsWorld();
    worlds[1] = new PhysicsWorld();
    //dynamicWorld = new PhysicsWorld();

    //whatIsIt->initializeObject();
    board->initializeObject();
    paddle1->initializeObject();
    sphere->initializeObject();
    sun->initializeObject();

    //physics init
    PhysicsWorld* worldDyn[2];//dynamic objects are not included in the second world, so don't hand it to them
    worldDyn[0] = worlds[0];
    worldDyn[1] = NULL;
    board->setTransforms(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(2.0,1.0,2.0), worlds, "ground", "mesh");
    glm::vec3 position = (glm::vec3(0,0,board->getCurrentMax().z) + glm::vec3(0,0,board->getCurrentCenter().z))/glm::vec3(2.0);
    paddle1->setTransforms(glm::vec3(0.0,0.0,0.0),//position + glm::vec3(0.0, 2.0, 0.0), 
        glm::vec3(0.0,0.0,0.0), 
        glm::vec3(1.1,1.1,1.1), 
        worlds, "static", "convex");//"puckY");
    sphere->setTransforms(glm::vec3(-1.2, 5.0, 0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(1.0,1.0,1.0), worldDyn, "dynamic", "sphere");
    //decided to move the paddle1 (pretending its a paddle)
    paddle1Trans = paddle1->nextTranslate;

    programShading = new Program(true, false, false);//normals, !color, !texture
    programTextures = new Program(true, false, true);//true);//normals, !color, texture (no blending)

    std::vector< Program* > programs = {programShading, programTextures};
    board->setPrograms(programs);
    paddle1->setPrograms(programs);
    sphere->setPrograms(programs);
    sun->setPrograms(programs);

    //Look at center of object:
    glm::vec3 center  = glm::vec3(board->getCurrentCenter());
    //glm::vec3 max = glm::vec3(board->getCurrentMax());//glm::vec3(board->max.x, board->max.y, board->max.z);
    //glm::vec3 min = glm::vec3(board->getCurrentMin());//glm::vec3(board->min.x, board->min.y, board->min.z);

    //Create Lights
    lights.push_back(Light("point"));
    //Add a default light; assume single light for now
    //A point Light
    lights[0].pos = glm::vec4(0.0f, 1.0f, 0.0, 1.0f);
    lights[0].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[0].diff = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
    lights[0].spec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights[0].inactive=false;
    lights[0].on = true;
    //An Ambient Light
    lights.push_back(Light("ambient"));
    lights[1].pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[1].amb = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    lights[1].on = true;
    lights[1].inactive=false;
    //A Spot Light
    lights.push_back(Light("spot"));
    lights[2].pos = glm::vec4(0.0, 2.0, 0.0, 1.0f); //note 1.0 makes directional
    lights[2].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[2].diff = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
    lights[2].spec = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
    lights[2].spotDir = glm::vec4(glm::vec4(0.0,0.0,0.0,1.0)-lights[2].pos);
    lights[2].constantAtten = 1.5;
    lights[2].linearAtten = 0.5;
    lights[2].quadraticAtten = 0.2;
    lights[2].spotCutoff = 45.0;
    lights[2].spotExp = 2.0;
    lights[2].on = true;
    lights[2].inactive=false;
    //A Distant Light
    lights.push_back(Light("distant"));
    lights[3].pos = glm::vec4(0.0f, 10.0f, 0.0f, 0.0f);
    lights[3].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[3].diff = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
    lights[3].spec = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights[3].on = true;
    lights[3].inactive=false;

    //default other lights: off
    lights.push_back(Light());//5 //4
    lights.push_back(Light());//6  //5
    lights.push_back(Light());//7  //6
    lights.push_back(Light());//8: Max Lights //7

    //std::cout<<"Light First: "<<light.pos[0]<<' '<<light.pos[1]<<' '<<light.pos[2]<<std::endl;

    //Add other lights...

    //ensure camera outside of object
    view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
                       center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
                       glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

    projection = glm::perspective(45.0f,  // the FoV typically 90 degrees is good which is what this is set to
                                  float(w)/float(h),  // Aspect Ratio, so Circles stay Circular
                                  0.01f,  // Distance to the near plane, normally a small value like this
                                  100.0f);  // Distance to the far plane,

    //update lights in program here, since we move them...
    //hmm: so...need to change Program and Light struct
    for (int i=0; i<lights.size(); i++) {
        programShading->addLight(lights[i], i);
        programTextures->addLight(lights[i], i);
    }
    programShading->addView(view);
    programTextures->addView(view);
    programShading->addProjection(projection);
    programTextures->addProjection(projection);

    // enable depth testing
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );

    // and its done
    return true;
}

void cleanUp() {
    // Clean up, Clean up
    delete programShading;
    delete programTextures;
    board->cleanUp();
    paddle1->cleanUp();
    sphere->cleanUp();
    //delete worlds last (so all objects already removed)
    delete worlds[0];
    delete worlds[1];
}

// returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
