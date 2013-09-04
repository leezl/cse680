Project 02: Interaction: Menus, Keyboard, and Mouse
========================================
This project is based on the example in the Torwegia-Matrix-Example.

CONTROLS:
Right Click: brings up menu: Start/Stop Spin and Quit
Left Click: Reverses Spin
Up Arrow/D: Increases Spin Speed
Down Arrow/A: Decreases Spin Speed (can go negative)
Space/S: Reverses Spin

The assigned alterations are to add the following:

1.) Menus: To learn more about adding menus, look in your book at Section 2.12 Menus. You'll be adding an option to start the spinning, to pause the spinning, and to exit the program.
A hint on starting/pausing the rotation: add a flag that is checked when the cube is rotated.

2.) Keyboard: To learn more about adding keyboard events, look in the book at Section 2.11.3 Keyboard Events. You will need to change the spinning of your cube based upon a key being pressed. For example if you press A, the rotation reverses direction.

3.) Mouse: Look in Section 2.11.1 Using the Pointing Device. You will be replicating what we did with the keyboard by using the mouse. If you click on the screen, the cube will reverse rotation

Added:    

    void createRotationMenu();
    void rotation_menu(int id);
    void rotation_speed(int key, int x, int y);
    void mouse_rotation(int button, int state, int x, int y);
    
Updated:   
    
    void keyboard(unsigned char key, int x_pos, int y_pos);
    int main();
    
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
