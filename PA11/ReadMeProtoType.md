Project 11, Midway: Labyrinth
========================================

Group Members
---------------------------
- Liesl Wigand
- Khoa Tran


Objectives:
---------------------------
*  Board, ball, knobs
*  Texture on Board and ball
*  Maze on Board
*  Holes/wining position marked
*  Basic Physics (ball rolls and bounces)
*  Multiple Lights
*  Movable Viewpoint (Camera)
*  Display Score
*  Menus [Functioning] (start, quit, pause)
*  Colors and textures
*  Controls to tilt the board
*  Clock (for scoring)
   
Description:   
----------------------------   
Tilt the board using keyboard/mouse controls to try to reach the goal. The goal appears as a cube, with one or more balls in the maze which need to reach the goal.  Once a ball reaches the goal it is removed from the board.
    
Controls:    
---------------------------------   
   
* ESC to close program    
   
Program Libraries Requirements
------------------------------------------------    
* This project requires cmake, GLM, glew, glut, devil, Assimp 3.0, and Bullet.   
* On Ubuntu it can be installed with the following commands:
    
   >$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev    
   >$ sudo apt-get install libglm-dev    
   >$ sudo apt-get install libtiff-dev libdevil-dev   

* For the Bullet library, it's recommended to follow the instruction at: http://bulletphysics.org/mediawiki-1.5.8/index.php/Installation and instructions from Bullet Manual in downloaded library.


How to Run Program
-------------------------------
Cmake project and program is located in bin

   >$ cd build    
   >$ cmake ..    
   >$ make  
   >$ cd ..    
   >$ cd bin    
   >$ ./pa

Note:
--------------------------
This project attempts to use cmake to create the Makefile. However, Bullet has proven difficult to find, even with a FindBULLET.cmake file.  If you're having problems with linking to the Bullet library, here are 2 solutions to link to the Bullet library.

* Method 1:  (Default) Link to Bullet library at: "/usr/local/include/bullet".  On line 19 of CMakeLists.txt, you should see "set (BULLET_SOURCE_DIR "/usr/local/include/bullet")".  This option should work if the instructions from http://bulletphysics.org/mediawiki-1.5.8/index.php/Installation and instructions from Bullet Manual in downloaded library was followed correctly.

* Method 2:  Direct link to builded library from downloaded library.  This method needs to have three variable set in the CMakeLists.txt:   
	- set( BULLET_ROOT "/home/liesl/Projects/bullet-2.81-rev2613")   
	- set (BULLET_BUILD_DIR "/home/liesl/Projects/bullet-2.81-rev2613/bulletBuild")   
	- set (BULLET_SOURCE_DIR "/usr/include/bullet")   
	You'll notice these lines 16-18 are commented out on the CmakeLists.txt.  Try this alternative method by replacing the location with your Bullet directory and comment out line 19.  The top two are paths to the bullet directory (ie. the extracted folder from the archive, and the second is a path into the build directory where cmake was run.) However, even this only worked if Bullet was built using: 
	> cmake .. -DINSTALL_LIBS=on -DBUILD_SHARED_LIBS=on
	So in build, using:    
	   > cmake ..    
	   > make    		  
Should result in the creation of the executable in the bin directory.    
    
    
Included in File:
---------------------------------------
- bin folder 
    ->assets
        ->shaders
        ->models
        ->blends (this should not be present in production)
- build folder
- cmake folder
    ->Modules: find_.cmake
- src folder
    ->_.h
    ->_.cpp
- Labyrinth_Manual.pdf
- CMakeLists.txt
- Readme.md
    
    
