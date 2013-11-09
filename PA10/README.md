Project 10: Lighting
========================================

Group Members
---------------------------
- Liesl Wigand
- Khoa Tran


Objective:
---------------------------
*  Use Phong-Blin Lighting in the shaders
*  Create Ambient Light
*  Create Point Light
*  Create Distant Light
*  Create Spot Light

Controls:
---------------------------------
  
* left arrow: camera gets closer to object    
* right arrow: camera gets farther from object    
* up arrow: raises camera    
* down arrow: lower camera    
* 1 Turns the Point Light on and off
* 2 Turns the Ambient Light on and off
* 3 Turns the Spot Light on and off
* 4 Turns the DIstant Light on and off
* i, k, J, L, I, K control the Point Light    
* j,l control the paddle left and right    
* a,s,d,w control the spot Light direction ........    
* A,S,D,W control the Spot parameters ........    
* ESC to close program     


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
    
