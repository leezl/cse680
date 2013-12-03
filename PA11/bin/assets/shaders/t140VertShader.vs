/*
Texturing and Shading vertex shader. version 1.40.
*/

//#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

attribute vec3 v_position;
//attribute vec3 v_normal;
attribute vec3 v_uv;

varying vec2 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(void){
    //positioning
    gl_Position = P * V * M * vec4(v_position, 1.0);

    texCoord = v_uv.xy;//get rid of extra for now

}
