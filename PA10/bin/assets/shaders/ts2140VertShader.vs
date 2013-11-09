/*
Texturing and Shading vertex shader. version 1.40.
*/

//#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec3 v_uv;

struct Light {
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    vec3 spotDirection;
};

varying vec2 texCoord;
varying vec4 position, E;
varying vec3 N;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 TIMAT;

void main(void){
   //positioning
    gl_Position = P * V * M * vec4(v_position, 1.0);
    position = V * M * vec4(v_position, 1.0);
    N = normalize(vec3(TIMAT * vec4(v_normal,0.0)));
    E = -normalize(V * M * vec4(v_position, 1.0));//V * M * v_position //negative?

    texCoord = v_uv.xy;//get rid of extra for now
}
