Project 08: Bullet Physics
========================================
Group: Khoa Tran, and Liesl Wigand
NOTE: We both have full working copies in our repositories this time around.

This project is based on the previous Project, PA07. It will only work in linux with a recent cmake. Also it relies on several libraries (see below under Building This Example).   
This project attempts to use cmake to create the Makefile. However, Bullet has proven difficult to find. Even with a FindBULLET.cmake file, it needs to have three variable set in the CMakeLists.txt:   
   set( BULLET_ROOT "/home/liesl/Projects/bullet-2.81-rev2613")   
   set (BULLET_BUILD_DIR "/home/liesl/Projects/bullet-2.81-rev2613/bulletBuild")   
   set (BULLET_SOURCE_DIR "/usr/include/bullet")   
   
The top two are paths to the bullet directory (ie. the extracted folder from the archive, and the second is a path into the build directory where cmake was run.) However, even this only worked if Bullet was built using: 
   cmake .. -DINSTALL_LIBS=on -DBUILD_SHARED_LIBS=on

So in build, using:    
   cmake ..    
   make    
      
Should result in the creation of the executable in the bin directory. See additional build help below.    
   
CONTROLS: 
left arrow: camera gets closer to object    
right arrow: camera gets farther from object    
up arrow: raises camera    
down arrow: lower camera    
i,k : move the cube up and down    
j,l : move the cube left and right       
    
The assigned alterations are to add the following:    
1.) Use Bullet Physics.   
2.) Have a cylinder, sphere, and cube on a board.   
3.) Have the cube and board be static, and the cylinder and sphere be dynamic.   
4.) Use btTriangleMeshes to load the collision objects? Extra Credit/ Graduate Credit.

    
The following is still true:    
    
Building This Example    
---------------------    
    
*This example requires GLM, glew, glut and devil. Also Assimp 3.0, sudo make installed from the src download. Bullet Physics should also be installed.*    
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
