Project 07: Assimp Textures
========================================
This project is based on the previous Project, PA06. It will only work in linux with a recent cmake. Also it relies on several libraries (see below under Building This Example).   
This project attempts to use cmake to create the Makefile. so in build, using:    
   cmake ..    
   make    
Should result in the creation of the executable in the bin directory. See additional build help below.    
   
CONTROLS:     
Space: starts/stops spinning of object    
a, d: change speed of spinning object    
left arrow: camera gets closer to object    
right arrow: camera gets farther from object    
up arrow: raises camera    
down arrow: lower camera    
s: scale object down    
S: scales object up
i,k : move the light up and down    
j,l : move the light left and right       
    
The assigned alterations are to add the following:    
1.) Load textures (from files)    
2.) Using Assimp to access the texture info for a mesh   
    
My program was broken for two days because I called deallocateTexture accidentally when copying textures incorrectly.    
Made a quick fix for this issue. Still not the cleanest way, but I am so glad to see things again.   
    
The following is still true:    
    
Building This Example    
---------------------    
    
*This example requires GLM, glew, glut and devil. Also Assimp 3.0, sudo make installed from the src download.*    
*On ubuntu it can be installed with this command*    
    
   >$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev    
   >$ sudo apt-get install libglm-dev    
   >$ sudo apt-get install libtiff-dev libdevil-dev   
    
To build this example just     
    
   >$ cd build    
   >$cmake ..    
   >$ make     
    
The excutable will be put in /bin.    
    
ALSO: I'm using a GLU error check, in case that gives any problems.   