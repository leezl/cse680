/*
Put all control functions here: mouse, keyboard etc
*/

// --Data types (other files now)

// --Evil Global variables
// Just for this example!
int w = 640, h = 480;  // Window size
glm::vec3 maxPos = glm::vec3(20.0,20.0,20.0);
glm::vec3 minPos = glm::vec3(-20.0,-20.0,-20.0);
//int w = 1280, h = 960;
//GLuint programShading, programTextures;  // The GLSL program handle
Program * programPlain=NULL;
Program * programShading=NULL;
Program * programTextures=NULL;
//std::vector<Object* > objects;
//Object *whatIsIt=NULL; //stores object. Yay. Gluint, Vertices, Indices, loader
Object *board=NULL;
Object *sphere=NULL;
Object *sphere2=NULL;
Object *sun=NULL;
Object *sky=NULL;
Object *goal=NULL;
PhysicsWorld* world;
Object *worldBounds;
Object *boardTop=NULL;
ObjectBillboard *timeDisplay=NULL;      //Display Time 
ObjectBillboard *topScoreDisplay1=NULL;     //top score display
ObjectBillboard *topScoreDisplay2=NULL;     
ObjectBillboard *topScoreDisplay3=NULL;     
ObjectBillboard *topScoreDisplay4=NULL;    
ObjectBillboard *topScoreDisplay5=NULL;     
ObjectBillboard *topScoreDisplayLabel=NULL;     
ObjectBillboard *gameOverDisplay1=NULL; 	//Message when game is over
ObjectBillboard *gameOverDisplay2=NULL; 	
//wasd, ijkl
//int keyBuffer[8] = {0};//not pressed //only controls

int glutTime0;      //Time0 - Added on project 11
int glutTime;       //Current Time - Added on project 11
int timePrevious = 0;   //Storage to find 1 sec time elapse
int seconds =0;
int minutes = 0;            //Seconds, minutes and hours for display
int hours = 0;
bool recordScore = true;	//Record score flag.  Reset flag to true when game is reset
//std::vector< std::vector<float> > scoreHistory;		//Score History Stack
std::vector<int> scoreHistoryHours;		//Score History Stack
std::vector<int> scoreHistoryMinutes;		
std::vector< float> scoreHistorySeconds;		
void UpdateScore(int gameOverHours, int gameOverMinutes, float gameOverTime);	//Keeps track of game score history

bool pauseFlag = false;
bool withTop = true;
bool twoBalls = false;
bool updateBall = false;
bool gameOver = false;
bool brightGame = true;

//----------------------------------------------------------------
//Camera control: dist,heigh,rotate
float dist = 25; //changed from -12 
float height = 15; //Changed from 6
float rotate = 0;
bool disableColor=true, disableTextures=false, aiEnabled=false;
bool disableOverheadCamera = true;	//Allows camera to follow ball.  Added for Project 11

//Maze
bool updateMaze=false;
int mazeChoice = 0;

// transform matrices
//std::vector<Objects> objects;  // stores model matices by buffer index
//move model matrix to object? well...
glm::mat4 view;  // world->eye
glm::mat4 projection;  // eye->clip
std::vector < Light > lights;

//Keyboard press
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)

//Mouse to tilt board
int previousMouseX = 0; //Mouse Control Storage
int previousMouseY = 0; //Mouse Control Storage
void mouseTilt(int x, int y);   //Use mouse to tilt board
bool disableMouseControl = true;	//Flag for mouse control

//Rotation constants and limits
float rotateX = 1.5;	//Rotation additional constant 
float rotateZ = 1.5;	
float rotateXLimit = 25; 	//Limit how much board can rotate
float rotateZLimit = 25;

void createMenu();
void menu(int id);      //Menu 
void keyDown(unsigned char key, int x_pos, int y_pos);
void keyUp(unsigned char key, int x_pos, int y_pos);
void keyOperations(void);
void arrow_keys(int key, int x, int y);
void restartGame();


inline bool file_exists_test (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
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
    if (keyStates['p']|| keyStates['P'] ) {//Return to camera centered at world 
        disableOverheadCamera = true;
      	dist = 25; 
      	height = 15; 
      	rotate = 0;
    }
    if (keyStates['o']|| keyStates['O'] ) {//Overhead Camera on ball
       disableOverheadCamera = false;
    }
    if (keyStates['m']) {//Mouse Control
       disableMouseControl = false;
    }
    if (keyStates['n']) {//Mouse Control
       disableMouseControl = true;
    }
    if (keyStates['g']) {//Force game over
       gameOver = true;
    }
  	float rotateX = 1.5;//3.4;
  	float rotateZ = 1.5;//3.4;	
  	float rotateXLimit = 25;
  	float rotateZLimit = 25;
    //Tilt Board
	  //Update board tilted position		
    if (keyStates['w'] || keyStates['W'] ) {//rotate board up
    		//Rotation Limit
    		if(board-> nextRotate.x < -rotateXLimit)
    		{
    	        board-> nextRotate.x -= 0; 
        	    goal->nextRotate.x  -= 0;
    		}	
    		else
    		{
    	        board-> nextRotate.x -= rotateX; 
        	    goal->nextRotate.x -= rotateX;
    		}
    }
    if (keyStates['s']|| keyStates['S'] ) {//rotate board down
    		//Rotation Limit
    		if(board-> nextRotate.x > rotateXLimit)
    		{
    	        board-> nextRotate.x += 0; 
        	    goal->nextRotate.x += 0;
    		}	
    		else
    		{
            	board-> nextRotate.x += rotateX;
            	goal-> nextRotate.x += rotateX;
    		}
    }

    if (keyStates['a']|| keyStates['A'] ) {//rotate board right
		//Rotation Limit
		if(board-> nextRotate.z > rotateZLimit)
		{
	        board-> nextRotate.z += 0; 
    	    goal->nextRotate.z  += 0;
		}	
		else
		{
	        board-> nextRotate.z += rotateZ; 
    	    goal->nextRotate.z += rotateZ;
		}
    }
    if (keyStates['d']|| keyStates['D'] ) {//rotate board left
		//Rotation Limit
		if(board-> nextRotate.z < -rotateZLimit)
		{
	        board-> nextRotate.z -= 0; 
    	    goal->nextRotate.z  -= 0;
		}	
		else
		{
	        board-> nextRotate.z -= rotateZ; 
    	    goal->nextRotate.z -= rotateZ;
		}
    }

    //EXAMPLE: continuous update: move left
    /*if (keyStates['k']) {//1
       paddle1Trans.z += 0.1;
    }*/
    //EXAMPLE: discrete update: turn on, turn off
    /*if (keyStates['1']) {//2
        //point
        lights[0].on = !lights[0].on;
        keyStates['1'] = false;
    }*/
}

void arrow_keys(int key, int x, int y) {
  // x and y are mouse position: how can we use this?

	if(disableOverheadCamera)	//Control to use arrows keys instead of ball position for camera parameter
	{
	  switch (key) {
		case GLUT_KEY_UP:
		  //if (height<30){
		    height += 1.0;
		    if (abs(height) > 35) {height = 35;}
		  //}
		  break;
		case GLUT_KEY_DOWN:
		  //if (height>-1){
		    height -= 1.0;
		    if (abs(height) > 35) {height = -35;}
		  //}
		  break;
		case GLUT_KEY_PAGE_UP:
		  //if (dist<30) {
		    dist += 1.0;
		    if (abs(dist) > 35) {dist = 35;}
		  //}
		  break;
		case GLUT_KEY_PAGE_DOWN:
		  //if (dist>1) {
		    dist -= 1.0;
		    if (abs(dist) > 35) {dist = -35;}
		  //}
		  break;
		case GLUT_KEY_LEFT:
		  //if (dist<30) {
		    //dist += 1.0;
		    rotate -=1.0;
		    if (abs(rotate) > 35) {rotate = -35;}
		  //}
		  break;
		case GLUT_KEY_RIGHT:
		  //if (dist>1) {
		    //dist -= 1.0;
		    rotate +=1.0;
		    if (abs(rotate) > 35) {rotate = 35;}
		  //}
		  break;
		//Resets camera
		case GLUT_KEY_HOME:
		    dist = 25; 
		    height = 15; 
		    rotate = 0;
		  break;
		}
	}
  //glutPostRedisplay();

  return;
}


//Mouse Motion Callback Function
// Updates cubeTrans based on position of mouse motion.  Uses 2 global variables - previousMouseX, previousMouseY
void mouseTilt(int x, int y) {

	if(!disableMouseControl)
	{
	/*
		//Tilt Up and down
		if(	previousMouseY > y)
		{
			board-> nextRotate.x -= rotateX; 
			goal->nextRotate.x -= rotateX;
			previousMouseY = y;
		}		
		else
		{
			board-> nextRotate.x += rotateX; 
			goal->nextRotate.x += rotateX;
			previousMouseY = y;
		}	

		//Tilt Left and Right
		if(	previousMouseX > x)
		{
			board-> nextRotate.z -= rotateZ; 
			goal->nextRotate.z -= rotateZ;
			previousMouseX = x;
		}		
		else
		{
			board-> nextRotate.z += rotateZ; 
			goal->nextRotate.z += rotateZ;
			previousMouseX = x;
		}*/
    /*
   	GLfloat depth;
	glReadPixels(x, h-y-25, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	glm::vec4 viewport = glm::vec4(0, 0, w,h);
	glm::vec3 wincoord = glm::vec3(x, h - y+50 , depth);

	glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);

	paddle1Trans = objcoord;

	glutPostRedisplay();
    */
		glutPostRedisplay();
	}

}

void createMenu() {
    int menu1, submenu;

    submenu = glutCreateMenu(menu);
    glutAddMenuEntry("Maze 1",5);
    glutAddMenuEntry("Maze 2",6);
    glutAddMenuEntry("Maze 3",7);

    menu1 = glutCreateMenu(menu);               //Menu
    glutAddMenuEntry("Pause/Resume",1);//combined pause and unpause
    glutAddMenuEntry("One Ball / Two Balls",2);
    glutAddMenuEntry("With Top / Without Top",3);
    glutAddMenuEntry("Bright / Dark",4); //Spotlights only: turns Distant Light off; dims skyBox?
    glutAddSubMenu("Switch Maze",submenu); //make into subMenu
    glutAddMenuEntry("Restart",9);
    glutAddMenuEntry("quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);  //Right Button Menu
}

void menu(int id) {
  switch(id)
  {
    //Pause
    case 1:
      pauseFlag = !pauseFlag;
      break;
    //Resume
    case 2:
      //updateBall = true;
      twoBalls = !twoBalls;
      if (twoBalls) {
        lights[4].on = true;
      }
      restartGame();
      break;
    case 3:
      withTop = !withTop;
      if (withTop) {
        boardTop->enablePhysics();
      } else {
        boardTop->disablePhysics();
      }
      break;
    case 4:
      brightGame = !brightGame;
      lights[1].on = !lights[1].on;
      break;
    case 5:
      if(mazeChoice!=0) {
        updateMaze = true;
        mazeChoice = 0;
      }
      break;
    case 6:
      if (mazeChoice!=1) {
        updateMaze = true;
        mazeChoice = 1;
      }
      break;
    case 7:
      if (mazeChoice!=2) {
        updateMaze = true;
        mazeChoice = 2;
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



void UpdateScore(int hour, int min, float sec)
{
	//float timeOld = (scoreHistoryHours[4] *60*12) + (60*scoreHistoryMinutes[4]) +scoreHistorySeconds[4];
	//float time = (hour *60*12) + (60*min) +sec;

	//Check if Score needs to be updated
	if(hour>=scoreHistoryHours[4] && min>=scoreHistoryMinutes[4] && sec>=scoreHistorySeconds[4])
	{
		std::cout<<"Score Not Recorded"<<std::endl;
	}
	else
	{
		//Temporary Variables
		int hoursTemp;
		int minutesTemp;		
		float secondsTemp;	
		bool copyFlag = false;
		//Swap old score into temp and insert new score.  Then swap back in
		for(int i =0; i< 5; i++)
		{

			if(hour<=scoreHistoryHours[i] && min<=scoreHistoryMinutes[i] && sec<=scoreHistorySeconds[i])
			{


				//Swap out
				hoursTemp = scoreHistoryHours[i];
				minutesTemp = scoreHistoryMinutes[i];
				secondsTemp = scoreHistorySeconds[i];

				//Swap in
				scoreHistoryHours[i] = hour;
				scoreHistoryMinutes[i] = min;
				scoreHistorySeconds[i] = sec;

				//Switch variables
				hour = hoursTemp;
				min = minutesTemp;
				sec = secondsTemp;
			}
			

		}

		std::cout<<"Score Recorded"<<std::endl;
	}


}

