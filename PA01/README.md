Project 01: shader loader and rotating cube
========================================
This project is based on the example in the Torwegia-Matrix-Example.

The assigned alterations are to
1.) Write a Shader Loader and move the shaders to files.
2.) Make the cube rotate about its Y-axis as well as move in the circle.

Added:    
    
    char* loadShader(char* filename);
    
Updated:   
    
    void update();   
    
Update now includes a rotation of the model. 
    
    void initialize();   
        
Iniltialize now use loadShader().   
    
    
The following is still true:

Building This Example
---------------------

*This example requires GLM*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install libglm-dev

To build this example just 

>$ cd build
>$ make

The excutable will be put in bin
