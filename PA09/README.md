
Project 09: Air Hockey
========================================

Group Members
---------------------------
- Liesl Wigand
- Khoa Tran


Objective:
---------------------------
*  Create an air hockey game with 2 paddles, a board, and a puck using everything learned from project 01 to 08
*  Air hockey game also contains:
*  Menu
*  AI
*  Mouse control for a paddle



Program Libraries Requirements
------------------------------------------------    
* This project requires cmake, GLM, glew, glut, devil, Assimp 3.0, and Bullet.   
* On Ubuntu it can be installed with the following commands:
    
   >$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev    
   >$ sudo apt-get install libglm-dev    
   >$ sudo apt-get install libtiff-dev libdevil-dev   

* For the Bullet library, it's recommended to follow the instruction at: http://bulletphysics.org/mediawiki-1.5.8/index.php/Installation and instructions from Bullet Manual in downloaded library


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
	Should result in the creation of the executable in the bin directory. See additional build help below.    


Included in File:
---------------------------------------
- bin folder 
- build folder
- cmake folder
- src folder
- Air Hockey Guide Manual pdf
- CMakeLists.txt
- Readme.md
   
In addition to the bin folder, there are object files and shaders in the assets folder

Controls:
---------------------------------
  
* Space: starts/stops spinning of object    
* a, d: change speed of spinning object    
* left arrow: camera gets closer to object    
* right arrow: camera gets farther from object    
* up arrow: raises camera    
* down arrow: lower camera    
* s: scale object down    
* S: scales object up
* Mouse control paddle 1
* i,j,k,l control paddle 1
* a,s,d,w control paddle 2
* "0" switches to puck 0
* "1" switches to puck 1
* ESC to close program   
    
Log:
------------------------------
*  Updated main and object file
*  Included new mesh, AI, and objectBillboard file.
    
=======
Project 09: Air Hockey   
========================================   
Group: Khoa Tran, and Liesl Wigand   
   
This isn't the repo you're looking for.   
   
   <a href="https://bitbucket.org/khoat/cs680">BitBucket Project 9</a>   
