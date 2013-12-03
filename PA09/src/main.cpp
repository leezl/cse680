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
Object *puck=NULL;
//Object *sphere=NULL;
//should be a better way of swapping these out, but kinda short on time
Object *paddle1=NULL;
Object *paddle2=NULL;
Object *sun=NULL;
PhysicsWorld* worlds[2];
Object *middle, *goal1, *goal2, *worldBounds;
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
glm::vec3 paddle2Trans = glm::vec3(0.0,0.0,0.0);


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

float rotationSpeed = 120.0;
bool rotateFlag=false;
float scaler = 1.0;
float dist = 25; //changed from -12
float height = 15; //Changed from 6
bool disableColor=true, disableTextures=false, aiEnabled=false;

//SCORE
int score[2] = {0};
//PADDLES
bool updatePaddle=false;
int paddleChoice = 0;
bool updatePuck=false;
int puckChoice = 0;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
//move model matrix to object? well...
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
Light light;
glm::vec4 lightposition;

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
    glutCreateWindow("Project 09: Air Hockey");
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
    //whatIsIt = new Object(path+"/", filename);
    board = new Object(path+"/", "assets/models/hockeyBoardStar.obj");
    paddle1 = new Object(path+"/", "assets/models/paddle2Blue.obj");
    paddle2 = new Object(path+"/", "assets/models/paddle1Red.obj");

    paddle1->addMesh(path+"/", "assets/models/paddle1Blue.obj");
    paddle2->addMesh(path+"/", "assets/models/paddle2Red.obj");

    puck = new Object(path+"/", "assets/models/moon.obj");
    puck->addMesh(path+"/", "assets/models/puck2.obj");

    //sphere = new Object(path+"/", "assets/models/sphere.obj");
    //whatIsIt->flipNormals();
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
	glutMotionFunc(mouseMove);			//Mouse Cursor Position
	glutPassiveMotionFunc(mouseMove);	//Removes the left button to move paddle
    createMenu();
	/*glutCreateMenu(menu);				//Menu
	glutAddMenuEntry("Pause",1);
	glutAddMenuEntry("Resume",2);
	glutAddMenuEntry("Toggle Between AI/2 Player",3);
	glutAddMenuEntry("Game Mode: Defensive AI",4);
	glutAddMenuEntry("Game Mode: Offensive AI",5);
	glutAddMenuEntry("Game Mode: Balanced AI",6);
	glutAddMenuEntry("Restart",9);
	glutAddMenuEntry("quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//Right Button Menu*/
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
    int menu,submenu;
    /*glutAddMenuEntry("Toggle Between AI/2 Player",3);
    glutAddMenuEntry("Game Mode: Defensive AI",4);
    glutAddMenuEntry("Game Mode: Offensive AI",5);
    glutAddMenuEntry("Game Mode: Balanced AI",6);*/
    submenu = glutCreateMenu(menuSub);
    glutAddMenuEntry("Disable AI",3);
    glutAddMenuEntry("Game Mode: Defensive AI",4);
    glutAddMenuEntry("Game Mode: Offensive AI",5);
    glutAddMenuEntry("Game Mode: Balanced AI",6);

    menu = glutCreateMenu(menuSub);               //Menu
    glutAddMenuEntry("Pause/Resume",1);//combined pause and unpause
    //glutAddMenuEntry("Resume",2);
    glutAddSubMenu("AI",submenu);
    glutAddMenuEntry("Swap Paddle", 7);
    glutAddMenuEntry("Swap Puck", 8);
    glutAddMenuEntry("Restart",9);
    glutAddMenuEntry("quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);  //Right Button Menu
}

void changePaddle() {
    //swap the paddle mesh
    paddle1->swapMesh(paddleChoice);
    paddle2->swapMesh(paddleChoice);
}

void changePuck() {
    puck->swapMesh(puckChoice);
}

// --Implementations
void render() {
    // --Render the scene

    // clear the screen
    glClearColor(0.2, 0.2, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw object
    board->drawObject();

    paddle2->drawObject();
    puck->drawObject();
    //sphere->drawObject();

    sun->drawObject();
	paddle1->drawObject();
	scoreText->drawObject();		//2D Score Update - New - Proj09
    /*
    int lenghOfString = (int)strlen(rotDirStr);
    glRasterPos2f(0.0-0.9, 0.0+0.9);
    for ( int i = 0; i < lenghOfString; i++ ) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, rotDirStr[i]);
    }
    */

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
    float dt = getDT(); 
    keyOperations();

    //EVEN IF PAUSED allow paddle swap
    if(updatePaddle==true) {
        updatePaddle=false;
        changePaddle();
    }

    if(updatePuck==true) {
        updatePuck=false;
        changePuck();
    }

	if(!pauseFlag)	//Menu Pause and Resume Option 
	{
        //std::cout<<dt<<','<<std::endl;
        worlds[0]->stepWorld(dt, 1);
        worlds[1]->stepWorld(dt, 1);

        board->updateModel();
        puck->updateModel();
        //sphere->updateModel();

        //keeps puck on board (after a fall returns)
        if (goal1->collidedWith) {
            score[1] += 1;
            goal1->collidedWith = false;
            std::cout<<"Score: "<<score[0]<<','<<score[1]<<std::endl;
        }
        if (goal2->collidedWith) {
            score[0] += 1;
            goal2->collidedWith = false;
            std::cout<<"Score: "<<score[0]<<','<<score[1]<<std::endl;
        }

		//move sun 
		sun->model = glm::translate(glm::mat4(1.0f), glm::vec3(light.pos.x, light.pos.y, light.pos.z));
		sun->model = glm::scale(sun->model, glm::vec3(0.15, 0.15, 0.15));

		//move view...NEED LIMITS HERE
		glm::vec3 center  = glm::vec3(board->getCurrentCenter()[0], board->getCurrentCenter()[1], board->getCurrentCenter()[2]);
		view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
		                   center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
		                   glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

		//----------------------------------------------------------------
		//New GLUT Function - Added for Proj09
		//----------------------------------------------------------------
		char text[256];
		sprintf(text,"Player 01 %2i        Player 02 %2i ", score[0], score[1] );
		scoreText->updateModel(text, 75,500, 20); //Display 2D Text score 
		
		//Simple AI
		//-----------------------------------------------------------------------------
		// - Move Paddle 1 and 2
		//-----------------------------------------------------------------------------

		//Paddle 1 Positions - center position
		glm::vec4 paddle1ObjectCoord = glm::vec4(0, 0, 0, 1.000000);
		glm::vec4 paddle1WorldPosition = (paddle1->model)*paddle1ObjectCoord;

		//Paddle 2 Positions - center position
		glm::vec4 paddle2ObjectCoord = glm::vec4(0, 0, 0, 1.000000);
		glm::vec4 paddle2WorldPosition = (paddle2->model)*paddle2ObjectCoord;

		//Board Position - Centered at player 2 goalie post
		glm::vec4 boardObjectCoord = glm::vec4(0, 0, 0, 1.0000);
		glm::vec4 boardWorldPosition = (board->model)*boardObjectCoord;

		//Puck Positions - center position 
		glm::vec4 puckObjectCoord = glm::vec4(.000000, 0.000000, 0.000000, 1.000000);
		glm::vec4 puckWorldPosition = (puck->model)*puckObjectCoord;

		//Update Paddle 1 and Paddle 2
        if (playerMode==1) {
        	ai->computePaddleMovement(	puckWorldPosition, 
        									boardWorldPosition, 
        									paddle1WorldPosition, 
        									paddle2WorldPosition,
        									playerMode,	//Player vs AI or Player 2
        									//"Defensive"
        									//"Offensive"
        									//"Balanced"
        									gameMode	//Def,Off,or Bal
        									);

        	paddle2->nextTranslate = ai->paddle2Trans;	//AI Movement Computation
        	paddle2->updateModel();
        	paddle2Trans = paddle2->nextTranslate;
        } else {
            paddle2->nextTranslate = paddle2Trans; //update user input
            paddle2->updateModel();
            paddle2Trans = paddle2->nextTranslate;
        }
        //mode player 1: non-ai player always
        paddle1->nextTranslate = paddle1Trans; //update user input
        paddle1->updateModel();
        paddle1Trans = paddle1->nextTranslate;

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
//act on key up or down
//should we call this in render or update? tutorial had it in render...
void keyOperations (void) {
    if (keyStates[27]) {
        // ESC
        exit(0);
    }
    if (keyStates['w']) {  
        paddle2Trans.z -= 0.1;
    }
    if (keyStates['a']) {  
        paddle2Trans.x -= 0.1;
    }
    if (keyStates['s']) {
        paddle2Trans.z += 0.1;
    }
    if (keyStates['d']) {
        paddle2Trans.x += 0.1;
    }
    if (keyStates['k']) {//1
       //lightposition.y -= 0.1;
       paddle1Trans.z += 0.1;
    }
    if (keyStates['i']) {//2
       //lightposition.y +=0.1;
        paddle1Trans.z -= 0.1;
    }
    if (keyStates['j']) {//1
       //lightposition.x -= 0.1;
        paddle1Trans.x -= 0.1;
    }
    if (keyStates['l']) {//2
       //lightposition.x +=0.1;
        paddle1Trans.x += 0.1;
    }
    if (keyStates['p']) {//Temporary force player 1 score
       score[0] = score[0]+1;
   }
    if (keyStates['1']) {
        updatePaddle = true;
        paddleChoice=0;
    }
    if (keyStates['2']) {
        updatePaddle = true;
        paddleChoice=1;
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

void restartGame() {
    //reset all:
    //score
    score[0]=0;
    score[1]=0;
    //puck
    puck->resetPosition();
    puck->updateModel();
    //paddle1
    //paddle1->resetPosition();
    //paddle2
    //paddle2->resetPosition();
}

//----------------------------------------------------------------
//New GLUT Function - Added for Proj09
//----------------------------------------------------------------

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
            updatePaddle = true;
            if (paddleChoice==1) {
                paddleChoice = 0;
            } else {
                paddleChoice =1;
            }
            break;
        case 8:
            updatePuck = true;
            if (puckChoice==1) {
                puckChoice = 0;
            } else {
                puckChoice =1;
            }
            break;
        //Restart
        case 9:
            restartGame();
            break;
        //Exit
        case 0:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

//Mouse Right Click Pop up Menu
void menu(int id) {
	switch(id)
	{
		//Pause
		case 1:
			pauseFlag = true;
			break;
		//Resume
		case 2:
			pauseFlag = false;
			break;
		//2 Players or 1 Player
		case 3:
			if(playerMode == 1)
			{
				playerMode = 2;
				gameStatusUpdate = true;
			}
			else
			{
				playerMode = 1;
				gameStatusUpdate = true;
			}
			break;
		//Defensive AI
		case 4:
			gameMode = "Defensive";
			gameStatusUpdate = true;
			break;		
		//Offensive AI
		case 5:
			gameMode = "Offensive";
			gameStatusUpdate = true;
			break;		
		//Balanced AI
		case 6:
			gameMode = "Balanced";
			gameStatusUpdate = true;
			break;	

		//Restart
		case 9:
            restartGame();
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
    paddle2->initializeObject();
    //sphere->initializeObject();
    puck->initializeObject();
    sun->initializeObject();

    //physics init
    PhysicsWorld* worldDyn[2];//dynamic objects are not included in the second world, so don't hand it to them
    worldDyn[0] = worlds[0];
    worldDyn[1] = NULL;
    board->setTransforms(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(2.0,1.0,2.0), worlds, "ground", "mesh");
    glm::vec3 position = (glm::vec3(0,0,board->getCurrentMax().z) + glm::vec3(0,0,board->getCurrentCenter().z))/glm::vec3(2.0);
    paddle1->setTransforms(position + glm::vec3(0.0, 2.0, 0.0), 
        glm::vec3(0.0,0.0,0.0), 
        glm::vec3(1.1,1.1,1.1), 
        worlds, "static", "convex");//"puckY");
    paddle2->setTransforms(-position + glm::vec3(0.0, 2.0, 0.0), 
        glm::vec3(0.0,0.0,0.0), 
        glm::vec3(1.1,1.1,1.1), 
        worlds, "static", "convex");//"puckY");
    //sphere->setTransforms(glm::vec3(-1.2, 5.0, 0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(0.5,0.5,0.5), worldDyn, "dynamic", "sphere");
    puck->setTransforms(glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(1.0,1.0,1.0), worldDyn, "dynamic", "convex");//"puckY");
    //decided to move the paddle1 (pretending its a paddle)
    paddle1Trans = paddle1->nextTranslate;
    paddle2Trans = paddle2->nextTranslate;

    //continuous collisions
    //default for now in setPhysics

    //set middle to be a collision object down center of board
    glm::vec4 tempCenter = board->getCurrentCenter();
    glm::vec4 tempMin = board->getCurrentMin();
    glm::vec4 tempMax = board->getCurrentMax();
    glm::vec3 middleMax = glm::vec3(tempMax);
    glm::vec3 middleMin = glm::vec3(tempMin.x, tempMin.y, tempCenter.z);
    middle = new Object(middleMax, middleMin, "middle");

    paddle1->setCollider(board, true, 0.7, true, false);//just get rid of this if half board works...
    paddle1->setCollider(middle, true, 0.9, true, false);
    paddle2->setCollider(board, true, 0.7, true, false);
    paddle2->setCollider(middle, false, 0.9, true, false);//outside the middle object
    //otherpaddle->setCollider(middle, false);//outside half board

    //make goal collision objects for the puck: on either end and down a bit
    //boardMax + (0.0, -1.0, 2.0) //player 1 Blue? Defends: score for player 2
    glm::vec3 goal1Max = glm::vec3(tempMax.x-0.1, tempMax.y, tempMax.z);//in a little from edge
    glm::vec3 goal1Min = glm::vec3(tempMin.x+0.1, tempMin.y, tempMax.z-0.3);//check scale here: in a little, but not too far: must collide, but not stick out of goal
    //boardMin - (0.0, -1.0, 2.0) //player 2 Red? Defends: score for player 1
    glm::vec3 goal2Max = glm::vec3(tempMax.x-0.1, tempMax.y, tempMin.z+0.3);
    glm::vec3 goal2Min = glm::vec3(tempMin.x+0.1, tempMin.y, tempMin.z);
    goal1 = new Object(goal1Max, goal1Min, "goal1");
    goal2 = new Object(goal2Max, goal2Min, "goal2");
    puck->setCollider(goal1, false, 0.0, false, true);
    puck->setCollider(goal2, false, 0.0, false, true);

    //set world bounds
    glm::vec3 boundsMax = maxPos;
    glm::vec3 boundsMin = minPos;
    worldBounds = new Object(boundsMax, boundsMin, "worldBound");
    puck->setCollider(worldBounds, true, 0.0, false, true);

    programShading = new Program(true, false, false);//normals, !color, !texture
    programTextures = new Program(true, false, true);//true);//normals, !color, texture (no blending)

    std::vector< Program* > programs = {programShading, programTextures};
    board->setPrograms(programs);
    paddle1->setPrograms(programs);
    paddle2->setPrograms(programs);
    puck->setPrograms(programs);
    //sphere->setPrograms(programs);
    sun->setPrograms(programs);

    //Look at center of object:
    glm::vec3 center  = glm::vec3(board->getCurrentCenter());
    glm::vec3 max = glm::vec3(board->getCurrentMax());//glm::vec3(board->max.x, board->max.y, board->max.z);
    glm::vec3 min = glm::vec3(board->getCurrentMin());//glm::vec3(board->min.x, board->min.y, board->min.z);
    //REmove following to change height manually
    //dist = max[2]+(0.5*(max[2]-min[2]));
    //height = max[1]+(0.5*(max[1]-min[1]));
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
    paddle2->cleanUp();
    puck->cleanUp();
    //sphere->cleanUp();
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
