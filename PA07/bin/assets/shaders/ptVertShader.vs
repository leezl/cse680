attribute vec3 v_position;
attribute vec3 v_color;

varying vec4 color;

uniform mat4 mvpMatrix;

void main(void){
    gl_Position = mvpMatrix * vec4(v_position, 1.0);
    color = vec4(v_color.rgb, 1.0);
}
