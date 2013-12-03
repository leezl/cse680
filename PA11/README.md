Project 11: Labyrinth
========================================

Group Members
---------------------------
- Liesl Wigand
- Khoa Tran

Final Update  (11/25/2013)
------------------------------------------
Current progress on Labyrinth:

* Board, ball, and knobs - Completed (Do we really need knobs?)
* Texture on board and ball  - Completed
* Maze on board  - Completed
* Holes/winning position marked  - Completed
* Basic physics   - Completed
* Multiple lights  - Completed (Ambient and Spot light included)
* Movable viewpoint (camera)  - Completed
* Display score  - Completed
* Menus (start, quit, pause)  - Completed
* Colors and textures  - Completed
* Controls to tilt the board   - Completed
* Clock (for scoring)  - Completed
* Multiple balls  - Completed (Only Two)
* Multiple levels  - Complete (Only Three)

Progress Update  (11/20/2013)
------------------------------------------
Current progress on Labyrinth:

* Board, ball, and knobs - Partially completed, knobs missing (Do we really need knobs?)
* Texture on board and ball  - Completed
* Maze on board  - Completed
* Holes/winning position marked  - Completed
* Basic physics   - Completed
* Multiple lights  - Completed (Ambient and Spot light included)
* Movable viewpoint (camera)  - Completed
* Display score  - Incompleted - inprogress
* Menus (start, quit, pause)  - Completed
* Colors and textures  - Completed
* Controls to tilt the board   - Completed
* Clock (for scoring)  - Completed
* Multiple balls  - Completed
* Multiple levels  - Incomplete

Extra Credit:

* Sky box - Completed
* Mouse controls - Partially completed - Sensitive control

Objectives:
---------------------------
*  Create a labyrinth game with textured board and ball.  Included features are lighting and physics.
    
Controls:    
---------------------------------   
   
* ESC to close program 
* a,s,w,d - board tilt
* arrow key up and arrow key down -  Change the height between the camera and board center
* arrow key left and arrow key right - Change the rotation between the camera and board center        
* Page Up and Page Down - Change the distance between the camera and board center     
   
Menu Controls:   
* Pause/Resume
* Restart
* Switch Maze
* Switch Lighting Bright or Dark
* Switch One or Two Balls
* Switch With Top, or Without
   
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

SKYBOX Courtesy of: FarmPeeeps.com; see bin/assets/models for more details
    
    
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

        
    
