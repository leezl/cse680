Project 04: Blender and Model Loading
========================================
This project is based on the previous Project, PA03.

CONTROLS:
Right Click: brings up menu: Start/Stop Spin and Quit
Left Click: Reverses Spin
Up Arrow/D: Increases Spin Speed
Down Arrow/A: Decreases Spin Speed (can go negative)
Left Arrow: Spins left
Right Arrow: Spins right
Space/S: Reverses Spin

The assigned alterations are to add the following:    
1.) Create a model in Blender and export the OBJ file.    
2.) Model Loading: Write a model loader that can load your OBJ from Part 1 into your program.    

Should have added:    
An object class for storing the model matrices, buffer indices, and display stuff.    
    
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
