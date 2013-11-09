

#include "AI.h"

AI::AI()	
{
	GameMode = "Defensive";
	Player = 1;
	puckMovementChangeCounter = 0;
	Paddle1Moved = false;
	Paddle2Move = false;
}


void AI::paddle1CheckMovement()
//--------------------------------------------------------------------------------------------
//Checked if paddle1 (player 1) moved - Takes 100 turns before determining if paddle1 moved
//Creates 2 stack and compares sum differences.  If differences is noticed, Paddle1Moved updates
{
	if(!Paddle1Moved)
	{
		//Create Motion History
		if(paddle1MovementHistory1.size()< 50)
		{
			paddle1MovementHistory1.push_back(abs(paddle1[0]));
			paddle1MovementHistory1.push_back(abs(paddle1[2]));
		}
		else
		{	
			if(paddle1MovementHistory2.size()> 50)
			{
				paddle1MovementHistory2.pop_front();
				paddle1MovementHistory2.pop_front();
			}
			paddle1MovementHistory2.push_back(paddle1[0]/paddle1[3]);
			paddle1MovementHistory2.push_back(paddle1[2]/paddle1[3]);
		}
		double paddle1Time1 = std::accumulate(paddle1MovementHistory1.begin(),paddle1MovementHistory1.end(),0);
		double paddle1Time2 = std::accumulate(paddle1MovementHistory2.begin(),paddle1MovementHistory2.end(),0);

		//Set flag if Paddle 1 moved
		if(abs(paddle1Time2-paddle1Time1) > 50 && paddle1MovementHistory2.size() >50)
		{
			Paddle1Moved = true;
			puckMovementChangeCounter = 0;
		}
	}

}
//--------------------------------------------------------------------------------------------




void AI::paddle2Offensive(float force)
//--------------------------------------------------------------------------------------------
//Paddle 2 hit pucks
//Overwrite paddle2Trans calculated by defense if offense mode set
// - force - force applied on puck based on travel distance
{
	
	//Debug
	//std::cout<<"Counter:    "<<puckMovementChangeCounter<<"Paddle2Move:  "<<Paddle2Move<<"Paddle1Moved:  "<<Paddle1Moved<<std::endl;

	//Offensive Mode Set
	if(Paddle2Move && Paddle1Moved)
	{
		//Find direction to move paddle by comparison
		float gradX = (paddle2[0]/paddle2[3]) >(puck[0]/puck[3]) ? -1*force:force;
		float gradZ = (paddle2[2]/paddle2[3]) >(puck[2]/puck[3]) ? -1*force:force;

		paddle2Trans.x = (paddle2[0])+(.1*gradX);
		paddle2Trans.z = (paddle2[2])+(.1*gradZ);

	}

}
//--------------------------------------------------------------------------------------------


void AI::puckCheckMovement(int delay)
//--------------------------------------------------------------------------------------------
//Check if Puck Moved
//  Puck must be in a complete stop
{
	//Check if pucked moved for last n updates
	int n = 25;
	double time1 = std::accumulate(puckMovementHistory.begin(),puckMovementHistory.end(),0);
	if(puckMovementHistory.size() >= n)
	{
		puckMovementHistory.pop_front();
		puckMovementHistory.pop_front();
	}
	puckMovementHistory.push_back(10*puck[0]);
	puckMovementHistory.push_back(10*puck[2]);
	double time2 = std::accumulate(puckMovementHistory.begin(),puckMovementHistory.end(),0);

	//Debug
	//std::cout<<"Time Differece: "<<time1-time2<<" Counter: "<<puckMovementChangeCounter<<" Paddle2Move: "<<Paddle2Move<<std::endl;
	
	if((time1 - time2) == 0)
	{
		puckMovementChangeCounter +=1;
		
	}
	else
	{
		puckMovementChangeCounter = 0;
	}


	if(puckMovementChangeCounter > delay)
	{
		Paddle2Move = true;
	}
	else
	{
		Paddle2Move = false;
	}
}



void AI::paddle2OffensiveFlag(float distance)
//--------------------------------------------------------------------------------------------
//Check if Paddle is near paddle
{
	float puckZ = puck[2]/puck[3]; 
	float separation = (puck[2]/puck[3]) - (paddle2[2]/paddle2[3]); 

	//Offensive Mode to set flags checks the following:
	//Checks for following conditions:
	// - Puck on player side?
	// - Puck on player side and distance between paddle2 and puck < distance?

	//Puck must not be on oponent side
	if(puckZ> 0)
	{	
		Paddle2Move =false;
		puckMovementChangeCounter = 0;
	}

	//Puck must not be on oponent side and distance between puck and paddle 2 < distance
	if(puckZ <0 && abs(separation) < distance)
	{
		Paddle2Move =true;
	}


	//std::cout<<" Counter: "<<puckMovementChangeCounter<<" Puck z: "<<puck[2] <<" Separation: "<< separation <<" Paddle2Move: "<<Paddle2Move<<std::endl;
}





void AI::computePaddleMovement(	glm::vec4 puckUpdate, 
								glm::vec4 boardUpdate, 
								glm::vec4 paddle1Update, 
								glm::vec4 paddle2Update,
								int playerMode = 1,
								std::string gameMode 
								)
//--------------------------------------------------------------------------------------------
//Updates variables in header file and computes translate of paddle2 and/or paddle1

/*
void AI::computePaddleMovement(	glm::vec4 puck, 
								glm::vec4 board, 
								glm::vec4 paddle1, 
								glm::vec4 paddle2,
								std::string gameMode = "Defensive",
								int playerMode = 1)*/
//Computes the movement of paddle 2
{
	//Update gameMode and playerMode
	puck = puckUpdate; 
	board = boardUpdate;
	paddle1 = paddle1Update; 
	paddle2 = paddle2Update;
	GameMode = gameMode;
	Player = playerMode;




	//Debug Output

	//std::cout<<std::setw(10)<<"Board:"<<std::setw(10)<<board[0]<<" "<<std::setw(10)<<board[1]<<" "<<std::setw(10)<<board[2]<<" "<<std::setw(10)<<board[3]<<std::endl;
	//std::cout<<std::setw(10)<<"Puck:"<<std::setw(10)<<puck[0]<<" "<<std::setw(10)<<puck[1]<<" "<<std::setw(10)<<puck[2]<<" "<<std::setw(10)<<puck[3]<<std::endl;
	//std::cout<<std::setw(10)<<"Paddle 1:"<<std::setw(10)<<paddle1[0]<<" "<<std::setw(10)<<paddle1[1]<<" "<<std::setw(10)<<paddle1[2]<<" "<<std::setw(10)<<paddle1[3]<<std::endl;
	//std::cout<<std::setw(10)<<"Paddle 2:"<<std::setw(10)<<paddle2[0]<<" "<<std::setw(10)<<paddle2[1]<<" "<<std::setw(10)<<paddle2[2]<<" "<<std::setw(10)<<paddle2[3]<<std::endl;

	//std::cout<<std::endl;

	// Set flag: Paddle1Moved
	paddle1CheckMovement();
	


	if(Player == 0) 
	{

	}
	else if(Player == 1)
	{
		
		//1 Player vs Offensive AI
		//Offensive mode will move back slowly then chase puck if in paddle's 2 zone
		if (GameMode.compare("Offensive") == 0)
		{
			
			//Paddle 2 Position as defense
			paddle2Trans.x = (puck[0]/puck[3]); 
			paddle2Trans.z = (puck[2]/puck[3])-5;
			//Passed Board center line - move paddle closer to puck
			if(puck[2]/puck[3] < 0)
			{
				paddle2Trans.x = puck[0]/puck[3];	 //Moves in x direction with puck
				paddle2Trans.z = paddle2Trans.z - .5*(puck[2]/puck[3]);
			}


			//Set Flag for offensive mode
			puckCheckMovement(50);
			paddle2OffensiveFlag(5);

			//Paddle 2 Offense
			paddle2Offensive(.5);
	
		}
		//1 Player vs Balance AI
		else if (GameMode.compare("Balanced") == 0)
		{
			//Balance - Stays mid-way between goal post and center of board most of the time
			paddle2Trans.x = puck[0]/puck[3];	 //Moves in x direction with puck
			paddle2Trans.z = (board[2]/board[3]) -8;
			
			//Passed Board center line - adjust defense mode
			if(puck[2]/puck[3] < 0)
			{
				paddle2Trans.x = puck[0]/puck[3];	 //Moves in x direction with puck
				paddle2Trans.z = paddle2Trans.z + .5*(puck[2]/puck[3]);
			}

	
			//Set Flag for offensive mode
			puckCheckMovement(50);
			paddle2OffensiveFlag(4);

			//Paddle 2 Offense
			paddle2Offensive(.5);
		}
		//1 Player vs Defensive AI
		else
		{
			//Defensive Play - Cover Goal Post

			paddle2Trans.x = puck[0]/puck[3]; //Moves in x direction with puck
			paddle2Trans.z = (board[2]/board[3]) -13;	//Block goal post based on z-direction on board
/*			
			if(abs(paddle2Trans.x) > 6)	
			{
				//Stays near goal post
				paddle2Trans.x = paddle2Trans.x > 0 ? 6:-6;
			}
*/
			//Check if puck moved - effect puckMovementChangeCounter variable
			puckCheckMovement(75);
			paddle2OffensiveFlag(3);

				
			//Paddle 2 Offense - Force Paddle 2 to push puck
			paddle2Offensive(.5);

		}
	}
	else
	{

	}
	

}








//Number of Player

