/*
Pass Through vertex Shader, version 1.40
*/

attribute vec3 v_position;

varying vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(void){
    gl_Position = P * V * M * vec4(v_position, 1.0);
    color = vec4(0.2, 0.2, 0.2, 1.0);
}
