Project 05: Assimp model loading
========================================
This project is based on the previous Project, PA04.   
    
Currently assumes that assimp is on a path indicated in the LD_LIBRARY_PATH variable.
   
CONTROLS:     
Space: stops spinning of object    
a, d: change speed of spinning object    
left arrow: camera gets closer to object    
right arrow: camera gets farther from object    
up arrow: raises camera    
down arrow: lower camera    
s: scale object down    
S: scales object up    

ALSO: I'm using a GLU error check, in case that gives any problems.
    
The assigned alterations are to add the following:    
1.)        
    
The following is still true:    
    
Building This Example
---------------------

*This example requires GLM*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev    
>$ sudo apt-get install libglm-dev    
    
To build this example just     
    
>$ cd build    
>$ make    
    
The excutable will be put in bin
