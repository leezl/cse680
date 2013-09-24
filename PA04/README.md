Project 04: Blender and Model Loading
========================================
This project is based on the previous Project, PA03.   

CONTROLS:    
Space: stops spinning of object
a, d: change speed of spinning object
m: camera gets closer to object
n: camera gets farther from object

Should be able to handle loading triangles and soem polygons. No fancy shading and a default color if no mtl is loaded. Does not use normals or uvs, though it does handle cases when they are present or not.
ALSO: I'm using a GLU error check, in case that gives any problems.
    
The assigned alterations are to add the following:    
1.) Create a model in Blender and export the OBJ file.    
2.) Model Loading: Write a model loader that can load your OBJ from Part 1 into your program.    
    
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
