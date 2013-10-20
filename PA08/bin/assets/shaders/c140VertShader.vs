/*
Simple, color only vertex shader, version 1.40
*/

attribute vec3 v_position;
attribute vec3 v_color;

varying vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(void){
    gl_Position = P * V * M * vec4(v_position, 1.0);
    color = vec4(v_color.rgb, 1.0);
}
