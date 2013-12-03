



#ifndef AI_
#define AI_


#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <numeric>	//For summing vectors

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  


#include <GL/glew.h>  // glew must be included before the main gl libs
#include <GL/glut.h>  // doing otherwise causes compiler shouting

/*
Simple AI that compute movements in the world coordinates
Options are: 0,1, or 2 player mode
AI Play Mode: 	defensive - covers goalie post and flick puck
				offensive - always chase puck
				balance - defensive and offensive
*/
class AI
{
	public:

	AI();	
	void computePaddleMovement(	glm::vec4 puck, 
								glm::vec4 board, 
								glm::vec4 paddle1, 
								glm::vec4 paddle2,
								int Player,
								std::string GameMode = "Defensive"
								);


	//Paddle Movements - Updated in computePaddleMovement
	glm::vec3 paddle1Trans; 
	glm::vec3 paddle2Trans; 

	private:
	void paddle1CheckMovement();					//Checks if paddle1 (player1) ever moved
	void paddle2Offensive(float force = .5);		//Offensive move for paddle1
	void puckCheckMovement(int delay = 100);		//Checks if Puck Moved and allow Paddle 2 to move
	void paddle2OffensiveFlag(float distance);		//Sets flag for offensive position based on game mode



	//Game Mode
	std::string GameMode;
	int Player;

	//Storage
	//-------------------------------------------------------------
	//Position of air hockey components
	glm::vec4 puck; 
	glm::vec4 board; 
	glm::vec4 paddle1; 
	glm::vec4 paddle2;

	//
	std::deque<float> paddle1MovementHistory1;
	std::deque<float> paddle1MovementHistory2;
	bool Paddle1Moved;							//Flag if player1 moved or not
	bool Paddle2Move;							//Flag to allow paddle2 to move
	//std::vector<float> paddle2MovementHistory;
	int puckMovementChangeCounter;
	std::deque<float> puckMovementHistory;

};




#endif
