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
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  // Makes passing matrices to shaders easier

#include "object.h"
#include "objectBillboard.h"	//Score Display
#include "AI.h"					//AI
#include "controls.h"

// --GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void setupLights();
void changeMaze();
// Added for Proj11
void top10DisplayDrawObject(int horizontalLoc, int verticalLoc, int textSize);
//---------------------------------------------------------------

// --Resource management
bool initialize();
void cleanUp();

// --Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;

// --Main
int main(int argc, char **argv) {
    //need filename for object file...
    std::string path="assets/models";

    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Project 11: Labyrinth");
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

    ilInit();//initialize devil
    iluInit();//more
    //ilutRenderer(ILUT_OPENGL);
    glActiveTexture( GL_TEXTURE0 );//not sure where this needs to be...
    //Load each object
	board = new Object(path+"/", "assets/models/squareMaze3NoNormKnobs.obj");//change this file later, when we have a board 
    board->addMesh(path+"/", "assets/models/roundMaze1NoNormKnobs.obj");
    board->addMesh(path+"/", "assets/models/board.obj");
    sphere = new Object(path+"/", "assets/models/ball1NoNorm.obj");//ball1NoNorm.obj");//change this file later when we have a ball
    sphere2 = new Object(path+"/", "assets/models/ball1NoNorm.obj");
    sky = new Object(path+"/", "assets/models/redoSky1.obj");
    goal = new Object(path+"/", "assets/models/cube.obj");
    boardTop = new Object(path+"/", "assets/models/cube.obj");
    //sky->flipNormals();
	//board = new Object(path+"/", "assets/models/board.obj");//squareMazNoNorm.obj");//change this file later, when we have a board 
	timeDisplay = new ObjectBillboard("assets/scoreboard/characters.tga");		//Display Time
	topScoreDisplay1=new ObjectBillboard("assets/scoreboard/characters.tga");	//Score History display
	topScoreDisplay2=new ObjectBillboard("assets/scoreboard/characters.tga");
	topScoreDisplay3=new ObjectBillboard("assets/scoreboard/characters.tga");
	topScoreDisplay4=new ObjectBillboard("assets/scoreboard/characters.tga"); 
	topScoreDisplay5=new ObjectBillboard("assets/scoreboard/characters.tga");
	topScoreDisplayLabel=new ObjectBillboard("assets/scoreboard/characters.tga");
	gameOverDisplay1 = new ObjectBillboard("assets/scoreboard/characters.tga");		//Display Message and score after game is over
	gameOverDisplay2 = new ObjectBillboard("assets/scoreboard/characters.tga");	

    //for now keep drawing the light
    if(file_exists_test("assets/models/sun.obj")) {
        sun = new Object("assets/models/", "assets/models/sun.obj");
    } else if (file_exists_test("assets/models/sphere.obj")) {
        sun = new Object("assets/models/", "assets/models/sphere.obj");
    } else {
        sun = new Object("defaultSunObj");
    }
    sun->flipNormals();//so it glows with the light inside

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
	glutMotionFunc(mouseTilt);			//Mouse Cursor Position
	glutPassiveMotionFunc(mouseTilt);	//Removes the left button to move paddle
    createMenu();

    // Initialize all of our resources(shaders, geometry)
	glutTime0 = glutGet(GLUT_ELAPSED_TIME);		//Get an initial time

    bool init = initialize();
    if ( init ) {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

void render() {
    // --Render the scene

    // clear the screen
    glClearColor(0.2, 0.2, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw object
    board->drawObject();
    if (!sphere->frozen) {
        sphere->drawObject();
    }
    if (twoBalls && !sphere2->frozen) {
        sphere2->drawObject();
    }
    sky->drawObject();
    goal->drawObject();
    //boardTop->drawObject();
	timeDisplay->drawObject();		//Display Time



	if(gameOver)
	{
		gameOverDisplay1->drawObject();		//Game over message with elapsed time
		gameOverDisplay2->drawObject();		

		//Display Game Top 5 Time
		topScoreDisplayLabel->drawObject();

		topScoreDisplay1->drawObject();
		topScoreDisplay2->drawObject();
		topScoreDisplay3->drawObject();
		topScoreDisplay4->drawObject();
		topScoreDisplay5->drawObject();

		//std::cout<<gameOver<<"  Game Over"<<std::endl;
	}


    //draw other lights: set sun position: drawObject, repeat...
    for (int i =0; i<8; i++) {
        //std::cout<<"Drawing  "<<i<<' '<<lights[i].type<<" what?"<<std::endl;
        if (lights[i].inactive == false && lights[i].type!="ambient" && lights[i].type!="distant" && lights[i].type!="point") {
            sun->model = glm::translate(glm::mat4(1.0f), glm::vec3(lights[i].pos.x, lights[i].pos.y, lights[i].pos.z));
            sun->model = glm::scale(sun->model, glm::vec3(0.40, 0.40, 0.40));
            sun->drawObject();
        }
    }

    glUseProgram(0);
    if (sphere->frozen) {
        //std::cout<<"Sphere 1 reached goal"<<std::endl;
        char *BallMsg = "Ball 1 is at the goal";
        int lenghOfString = (int)strlen(BallMsg);
        glRasterPos2f(0.0-0.9, 0.0+0.95);
        for ( int i = 0; i < lenghOfString; i++ ) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, BallMsg[i]);
            //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, GameWonMessage[i]);
        }
    }
    if (twoBalls && sphere->frozen) {
        //std::cout<<"Sphere 2 reached goal"<<std::endl;
        char *BallMsg2 = "Ball 2 is at the goal";
        int lenghOfString = (int)strlen(BallMsg2);
        glRasterPos2f(0.0-0.9, 0.0+0.90);
        for ( int i = 0; i < lenghOfString; i++ ) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, BallMsg2[i]);
            //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, GameWonMessage[i]);
        }
    }

    // swap the buffers
    glutSwapBuffers();
}

void update() {
    //update light position

    //std::cout<<"Light in main "<<light.pos.x<<','<<light.pos.y<<','<<light.pos.z<<std::endl;
    float dt = getDT();
    if(updateMaze==true) {
        updateMaze=false;
        changeMaze();
    }

	if(!pauseFlag)	//Menu Pause and Resume Option 
	{
        keyOperations();
        //std::cout<<dt<<','<<std::endl;
        world->stepWorld(dt, 1);

        board->updateModel();
        sphere->updateModel();
        if (twoBalls) {
            sphere2->updateModel();
        }

        //point the spot light at the ball
        lights[2].spotDir = glm::vec4(glm::vec4(sphere->nextTranslate, 1.0)-lights[2].pos);
        if (twoBalls) {
            lights[4].spotDir = glm::vec4(glm::vec4(sphere2->nextTranslate, 1.0)-lights[4].pos);
        }

        //determine goal position
        glm::mat4 temp = glm::mat4(1.0);
        glm::vec3 whereStop = glm::vec3(board->model * glm::vec4(goal->nextTranslate, 1.0));
        temp = glm::translate(temp, whereStop);
        goal->model = glm::scale(temp, goal->nextScale);

        //we want the original translate and scale to remain the same. We want to add to the rotation.
        if (withTop) {
            boardTop->nextRotate = board->nextRotate;
            boardTop->updateModel();
        }

        //check collisions with goal:
        if (goal->collidedWith) {
            goal->collidedWith = false;
            //check that both sphere's have reached goal
            if (sphere->frozen) {
                if(twoBalls && sphere2->frozen) {
                    gameOver = true;
                    pauseFlag = true;
                } else if (!twoBalls){
                    gameOver = true;
                    pauseFlag = true;
                }
            }

        }
        BoardConfig tempBoard = board->getBoardStop();
        if (sphere->reset) {
            glm::vec3 whereStart = glm::vec3(board->model * glm::vec4(tempBoard.startLocation, 1.0));
            sphere->resetPosition( whereStart, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4) );
            sphere->reset =false;
        }
        if (sphere2->reset) {
            glm::vec3 whereStart2 = glm::vec3(board->model * glm::vec4(tempBoard.startLocation2, 1.0));
            sphere2->resetPosition( whereStart2, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4) );
            sphere2->reset =false;
        }

		//Display Time
		//------------------------------------------------------------------------------------------
		char text[256];
		//sprintf(text,"Player 01 %2i        Player 02 %2i ", score[0], score[1] );
		glutTime = glutGet(GLUT_ELAPSED_TIME);
		float deltaTime =  glutTime-glutTime0;
		float time = deltaTime/1000;

		//Updates on 1 seconds elapse only.  Ignores milliseconds
		if( abs( (int)time - timePrevious) == 1)
		{
			seconds +=1;
			timePrevious += 1;
		}
		//Update Minute
		if(seconds > 59)	
		{
			minutes +=1;
			seconds = 0;

		}
		//Update Hour
		if(minutes > 59)	
		{
			hours +=1;
			minutes = 0;
		}	
		//Used to display seconds and milliseconds  (Optional)
		if(minutes > 0)
		{
			time = (time-(int)time)+seconds;
		}	

		sprintf(text,"Time Elapsed: %i:%i:%f", hours, minutes,time );
		timeDisplay->updateModel(text, 75, 500, 20); //Display 2D Text score 

		//Game Over Case
		//1.)  Create Message for Game Over
		//2.)  Updates game history
		if( gameOver && recordScore)
		{
			char textGameOver[256];
			int gameOverHours = hours;
			int gameOverMinutes = minutes;
			float gameOverTime = time;
			recordScore = false;

			//Display Final Score after Game Over
			sprintf(textGameOver,"Congratulations!");
			gameOverDisplay1->updateModel(textGameOver, 150, 280, 30); //Display 2D Text score 
			sprintf(textGameOver,"Your time was: %i:%i:%f", gameOverHours, gameOverMinutes,gameOverTime);
			gameOverDisplay2->updateModel(textGameOver,75 , 250, 25); //Display 2D Text score 

			//Update Score History
			UpdateScore(gameOverHours,gameOverMinutes,gameOverTime);
		}

		//Display Score History
		sprintf(text, "Top 5 Scores (Time):");
		topScoreDisplayLabel->updateModel(text, 10, 150, 20);

		sprintf(text, "%i:%i:%f", scoreHistoryHours[0], scoreHistoryMinutes[0],scoreHistorySeconds[0] );
		topScoreDisplay1->updateModel(text, 10, 120, 20);
		sprintf(text, "%i:%i:%f", scoreHistoryHours[1], scoreHistoryMinutes[1],scoreHistorySeconds[1]  );
		topScoreDisplay2->updateModel(text, 10, 100, 20);
		sprintf(text, "%i:%i:%f", scoreHistoryHours[2], scoreHistoryMinutes[2],scoreHistorySeconds[2]  );
		topScoreDisplay3->updateModel(text, 10, 80, 20);
		sprintf(text, "%i:%i:%f", scoreHistoryHours[3], scoreHistoryMinutes[3],scoreHistorySeconds[3]  );
		topScoreDisplay4->updateModel(text, 10, 60, 20);
		sprintf(text, "%i:%i:%f", scoreHistoryHours[4], scoreHistoryMinutes[4],scoreHistorySeconds[4]  );
		topScoreDisplay5->updateModel(text, 10, 40, 20);


		//Overhead camera
		if(!disableOverheadCamera)
		{

      		height = 10 + abs(board ->nextRotate.x)/4; 

			//Lower Left Corner of Board			
			if(sphere->nextTranslate.x < 0 &&  sphere->nextTranslate.z >0 )
			{
				dist = 15;
      			rotate = -10;
			}
			//Lower Right Corner of Board
			if(sphere->nextTranslate.x > 0 &&  sphere->nextTranslate.z >0 )
			{
				dist = 15;
      			rotate = 10 ;
			}
			//Upper Left Corner of Board
			if(sphere->nextTranslate.x < 0 &&  sphere->nextTranslate.z <0 )
			{
				dist = -15;
      			rotate = -10 ;
			}
			//Upper Right Corner of Board
			if(sphere->nextTranslate.x > 0 &&  sphere->nextTranslate.z <0 )
			{
				dist = -15;
      			rotate = 10 ;
			}

			view = glm::lookAt(glm::vec3(rotate, height, dist),  // Eye Position
				               glm::vec3(sphere->nextTranslate.x, sphere->nextTranslate.y, sphere->nextTranslate.z),  // Focus point
				               glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up
		}	
		else
		{	//Set Camera
			view = glm::lookAt(glm::vec3(rotate, height, dist),  // Eye Position
				               glm::vec3(0.0, 0.0, 0.0),  // Focus point
				               glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up
		}
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

void restartGame() {
    //reset all:
    //board
    board->resetPosition();
    //ball(s)
    BoardConfig tempBoard = board->getBoardStop();
    glm::vec3 whereStart = glm::vec3(board->model * glm::vec4(tempBoard.startLocation, 1.0));
    glm::vec3 whereStart2 = glm::vec3(board->model * glm::vec4(tempBoard.startLocation2, 1.0));
    sphere->resetPosition( whereStart, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4) );
    if (twoBalls) {
        sphere2->resetPosition( whereStart2, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4) );
    }
    sphere->frozen = false;
    sphere2->frozen = false;
    //reset goal position
    goal->nextTranslate = tempBoard.stopLocation;
    glm::vec3 whereStop = glm::vec3(board->model * glm::vec4(tempBoard.stopLocation, 1.0));
    glm::mat4 temp = glm::mat4(1.0);
    temp = glm::translate(temp, whereStop);
    //goal->nextTranslate = whereStop;
    //goal should be rotated with the board
    temp = glm::rotate( temp, board->nextRotate.x, glm::vec3(1.0,0.0,0.0) );
    temp = glm::rotate( temp, board->nextRotate.z, glm::vec3(0.0,0.0,1.0) );
    goal->model = glm::scale(temp,goal->nextScale);
    //reset boardTop
    glm::vec3 boardMax = glm::vec3(board->getCurrentMax());
    glm::vec3 boardCenter = glm::vec3(board->getCurrentCenter());
    glm::vec3 boardTopMax = glm::vec3(boardTop->getGlobalMax());
    glm::vec3 boardTopCenter = glm::vec3(boardTop->getGlobalCenter());
    glm::vec3 boardTopScale = glm::vec3((boardMax.x+1.0-boardCenter.x)/(boardTopMax.x-boardTopCenter.x), 
                                        (boardMax.y-boardCenter.y)/(boardTopMax.y-boardTopCenter.y), 
                                        (boardMax.z+1.0-boardCenter.z)/(boardTopMax.z-boardTopCenter.z));
    boardTop->resetPosition(glm::vec3(board->nextTranslate.x, 1.5*(boardMax.y-boardCenter.y), board->nextTranslate.z), 
                            board->nextRotate,
                            boardTopScale);
    if (!withTop) {
        boardTop->disablePhysics();
    }
    //reset flags and score
	recordScore = true;	//Flag to record score to be display and stored for top 5.
    //reset score to 0/ time to 0
    hours = minutes = seconds = 0;
    //reset game over flag
	gameOver = false;
	glutTime0 =  glutGet(GLUT_ELAPSED_TIME);	//Reset Initial Time
}

void changeMaze() {
    //swap the mesh for the maze
    board->swapMesh(mazeChoice);
    //restart the game
    restartGame();
}
//TODO: may not need this: chekc and DELETE
void changeBallMode() {
    //restart the game
    restartGame();
}



void setupLights() {
    //Create Lights: This is a lot, but we can take some out later
    lights.push_back(Light("point"));
    //Add a default light; assume single light for now
    //A point Light
    lights[0].pos = glm::vec4(0.0f, 5.0f, 0.0, 1.0f);
    lights[0].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[0].diff = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    lights[0].spec = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    lights[0].inactive=false;
    lights[0].on = false;
    //An Ambient Light
    lights.push_back(Light("ambient"));
    lights[1].pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[1].amb = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    lights[1].on = false;
    lights[1].inactive=false;
    //A Spot Light
    lights.push_back(Light("spot"));
    lights[2].pos = glm::vec4(0.0, 7.0, 0.0, 1.0f); //note 1.0 makes directional
    lights[2].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[2].diff = glm::vec4(30.0f, 15.0f, 10.0f, 1.0f);
    lights[2].spec = glm::vec4(30.0f, 15.0f, 10.0f, 1.0f);
    lights[2].spotDir = glm::vec4(glm::vec4(sphere->nextTranslate, 1.0)-lights[2].pos);
    lights[2].constantAtten = 1.5;
    lights[2].linearAtten = 0.5;
    lights[2].quadraticAtten = 0.2;
    lights[2].spotCutoff = 15.0;
    lights[2].spotExp = 2.0;
    lights[2].on = true;
    lights[2].inactive=false;
    //A Distant Light
    lights.push_back(Light("distant"));
    lights[3].pos = glm::vec4(0.0f, 5.0f, 0.0f, 0.0f);
    lights[3].amb = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    lights[3].diff = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    lights[3].spec = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    lights[3].on = true;
    lights[3].inactive=false;

    //A Spot Light
    lights.push_back(Light("spot"));
    lights[4].pos = glm::vec4(0.0, 7.0, 0.0, 1.0f); //note 1.0 makes directional
    lights[4].amb = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[4].diff = glm::vec4(30.0f, 15.0f, 10.0f, 1.0f);
    lights[4].spec = glm::vec4(30.0f, 15.0f, 10.0f, 1.0f);
    lights[4].spotDir = glm::vec4(glm::vec4(sphere2->nextTranslate, 1.0)-lights[4].pos);
    lights[4].constantAtten = 1.5;
    lights[4].linearAtten = 0.5;
    lights[4].quadraticAtten = 0.2;
    lights[4].spotCutoff = 15.0;
    lights[4].spotExp = 2.0;
    if(twoBalls) {
        lights[4].on = true;
    } else {
        lights[4].on = false;
    }
    lights[4].inactive=false;

    //default other lights: off
    //lights.push_back(Light());//5 //4
    lights.push_back(Light());//6  //5
    lights.push_back(Light());//7  //6
    lights.push_back(Light());//8: Max Lights //7
}

bool initialize() {
    // Initialize basic geometry and shaders for this example
    world = new PhysicsWorld();

    //whatIsIt->initializeObject();
    board->initializeObject();
    boardTop->initializeObject();
    sphere->initializeObject();
    sphere2->initializeObject();
    sky->initializeObject();
    goal->initializeObject();
    sun->initializeObject();

    //physics init
    //may need to adjust the setTransforms function; if the ground doesn't move, thats my fault.
    board->setTransforms( glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,0.0), glm::vec3(3.0,3.0,3.0), world, "static", "mesh" );
    sky->model = glm::scale( glm::mat4(1.0), glm::vec3(4.0,4.0,4.0) );//glm::rotate( glm::mat4(1.0), 180.0f, glm::vec3(1.0, 0.0, 0.0) ), glm::vec3(3.0, 3.0, 3.0) );
    glm::vec3 boardMax = glm::vec3(board->getCurrentMax());
    glm::vec3 boardCenter = glm::vec3(board->getCurrentCenter());
    glm::vec3 boardTopMax = glm::vec3(boardTop->getCurrentMax());
    glm::vec3 boardTopCenter = glm::vec3(boardTop->getCurrentCenter());
    glm::vec3 boardTopScale = glm::vec3((boardMax.x+1.0-boardCenter.x)/(boardTopMax.x-boardTopCenter.x), 
                                        (boardMax.y-boardCenter.y)/(boardTopMax.y-boardTopCenter.y), 
                                        (boardMax.z+1.0-boardCenter.z)/(boardTopMax.z-boardTopCenter.z));
    boardTop->setTransforms( glm::vec3(board->nextTranslate.x, 1.5*(boardMax.y-boardCenter.y), board->nextTranslate.z), board->nextRotate, boardTopScale, world, "static", "convex" );
    //determine goal position
    glm::mat4 temp = glm::mat4(1.0);
    BoardConfig tempBoard = board->getBoardStop();
    //goal should be translated to the boards: stop position
    //std::cout<<tempBoard.stopLocation.x<<','<<tempBoard.stopLocation.y<<','<<tempBoard.stopLocation.z<<std::endl;
    goal->nextTranslate = tempBoard.stopLocation;
    glm::vec3 whereStop = glm::vec3(board->model * glm::vec4(tempBoard.stopLocation, 1.0));
    //std::cout<<whereStop.x<<','<<whereStop.y<<','<<whereStop.z<<std::endl;
    temp = glm::translate(temp, whereStop);
    //goal should be rotated with the board
    temp = glm::rotate( temp, board->nextRotate.x, glm::vec3(1.0,0.0,0.0) );
    temp = glm::rotate( temp, board->nextRotate.z, glm::vec3(0.0,0.0,1.0) );
    //goal should be roughly the size of the ball
    //dist from center to max:
    float goalRadius = glm::distance(goal->getCurrentMax(), goal->getCurrentCenter());
    //dist from ball's center to max //following same for both balls
    sphere->nextScale = glm::vec3(0.4,0.4,0.4);
    sphere->model = sphere->getModel();
    float ballRadius = glm::distance(sphere->getCurrentMax(), sphere->getCurrentCenter());
    //proportional change needed //This scaling does not look right yet....
    glm::vec3 scaleToBall  = glm::vec3(ballRadius/goalRadius);
    goal->nextScale = scaleToBall;
    goal->model = glm::scale(temp, scaleToBall);

    //set sphere start location
    glm::vec3 whereStart = glm::vec3(board->model * glm::vec4(tempBoard.startLocation, 1.0));
    sphere->setTransforms( whereStart, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4), world, "dynamic", "sphere" ); 
    glm::vec3 whereStart2 = glm::vec3(board->model * glm::vec4(tempBoard.startLocation2, 1.0));
    sphere2->setTransforms( whereStart2, glm::vec3(0.0,0.0,0.0), glm::vec3(0.4,0.4,0.4), world, "dynamic", "sphere" );

    //set world bounds: sphere still reset if it leaves the world
    glm::vec3 boardRadius = boardMax-boardCenter;
    //FOllowing assumes board at center of world
    glm::vec3 boundsMax = glm::vec3(std::max(boardRadius.x, std::max(boardRadius.y, boardRadius.z))+5.5,
                                    std::max(boardRadius.x, std::max(boardRadius.y, boardRadius.z))+5.5,
                                    std::max(boardRadius.x, std::max(boardRadius.y, boardRadius.z))+5.5);//maxPos;
    glm::vec3 boundsMin = glm::vec3(std::min(-boardRadius.x, std::min(-boardRadius.y, -boardRadius.z))-5.5,
                                    std::min(-boardRadius.x, std::min(-boardRadius.y, -boardRadius.z))-5.5,
                                    std::min(-boardRadius.x, std::min(-boardRadius.y, -boardRadius.z))-5.5);//minPos;
    worldBounds = new Object(boundsMax, boundsMin, "worldBound");
    sphere->setCollider(worldBounds, true, 0.0, false, true, false);
    sphere->setCollider(goal, false, 0.0, false, false, true);
    sphere2->setCollider(worldBounds, true, 0.0, false, true, false);
    sphere2->setCollider(goal, false, 0.0, false, false, true);

    //Programs
    programPlain = new Program(false, false, true, false);//for sky box: no shading
    programShading = new Program(true, false, false, true);//normals, !color, !texture
    programTextures = new Program(true, false, true, true);//true);//normals, !color, texture (no blending)

    std::vector< Program* > programs = {programPlain, programShading, programTextures};
    board->setPrograms(programs); //choose your own shader
    boardTop->setPrograms(programs);
    sphere->setPrograms(programs); //set this exact shader
    sphere2->setPrograms(programs); //set this exact shader
    goal->setProgram(programShading);
    sun->setProgram(programShading);
    //select program for sky manually
    //sky->setProgram(programPlain);
    sky->setPrograms(programs);

    //Look at center of object:
    glm::vec3 center  = glm::vec3(board->getCurrentCenter());
    
    setupLights();

    //ensure camera outside of object
    view = glm::lookAt(glm::vec3(0.0, height, dist),  // Eye Position
                       center,//glm::vec3(0.0, 0.0, 0.0),  // Focus point
                       glm::vec3(0.0, 1.0, 0.0));  // Positive Y is up

    projection = glm::perspective(45.0f,  // the FoV typically 90 degrees is good which is what this is set to
                                  float(w)/float(h),  // Aspect Ratio, so Circles stay Circular
                                  0.01f,  // Distance to the near plane, normally a small value like this
                                  100.0f);  // Distance to the far plane,


    //update lights in program here, since we move them...
    for (int i=0; i<lights.size(); i++) {
        programShading->addLight(lights[i], i);
        programTextures->addLight(lights[i], i);
    }
    programPlain->addView(view);
    programShading->addView(view);
    programTextures->addView(view);
    programPlain->addProjection(projection);
    programShading->addProjection(projection);
    programTextures->addProjection(projection);

    // enable depth testing
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );


	//Set Default score history
	for (int i =0; i <5; i++)
	{
		scoreHistoryHours.push_back(99);
		scoreHistoryMinutes.push_back(99);
		scoreHistorySeconds.push_back(99);
	}
    // and its done
    return true;
}

void cleanUp() {
    // Clean up, Clean up
    delete programShading;
    delete programTextures;
    board->cleanUp();
    sphere->cleanUp();
    sphere2->cleanUp();
    sun->cleanUp();
    sky->cleanUp();
    //delete worlds last (so all objects already removed)
    delete world;
}

// returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
