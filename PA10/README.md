See Other Repo:

git@bitbucket.org:khoat/cs680.git


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
   
Description:   
----------------------------   
There are spheres drawn for each of the lights, which have their normals flipped. This is so they show the lights inside them best, but it makes their shading look wrong. Ambient Lights do not have spheres drawn.  
Initially:
1 Point Light On and Active
2 Ambient Light on 
3 Spot Light on
4 Distant Light on

When first loaded the Point Light tied to 1 is active, so it will move with wasdWS. Each of 8 possible lights can be turned off and on with their numbers: 1,2,3,4 are on at the start. More controls are below.
   
Controls:   
---------------------------------   
  
* left arrow: camera gets closer to center    
* right arrow: camera gets farther from center    
* up arrow: raises camera    
* down arrow: lower camera   
* i,j,k,l control the blue paddle still  
	-> i forward
	-> j left 
	-> k backward
	-> l right
* ESC to close program    
   
* 1 Turns the Point Light on and off   
* 2 Turns the Ambient Light on and off  
* 3 Turns the Spot Light on and off  
* 4 Turns the Distant Light on and off   
* Other number up to 8 turn off and on added SpotLights   

* Add more Spot Lights with '+' key   
* Swap the Active Light with the space bar   
The Following Controls the active Light:    
* a,s,d,w,W,S control the Light position   
	-> w forward   
	-> a left   
	-> s backward   
	-> d right   
	-> W up   
	-> S down  
*  If the light is a spot light: (Here's where things get pointlessly complicated)   
	-> A,D control spotlight cutoff angle   
	-> q,e control the x spotlight direction   
	-> z,c control the z spotlight direction   
	-> Q,E control the spotlight exponent
   

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
    
