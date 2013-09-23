Project 04: Blender and Model Loading
========================================
This project is based on the previous Project, PA03.   

Current reads in the filename with cin...probably get to chaning that before turn in.
    
The assigned alterations are to add the following:    
1.) Create a model in Blender and export the OBJ file.    
2.) Model Loading: Write a model loader that can load your OBJ from Part 1 into your program.    
    
SO...the program tries to point the camera at the object wherever it is. But it doesn't back up at the moment (ie the camera may appear inside the object).   
The object class tries to handle loading storing, and drawing the object. Kind of inefficient.
    
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
