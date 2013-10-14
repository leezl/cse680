/*
Texturing and Shading fragment shader. version 3.30 for practice and comparison.
*/

#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

in vec3 v_position;
in vec3 v_normal;
in vec3 v_uv;

out vec4 color;
out vec2 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec4 LightPosition;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform vec4 AmbientProduct;
uniform float Shininess;

void main(void){
    vec4 diffuse, specular, ambient;

    //positioning
    //vec3 pos = (V * M * vec4(v_position, 1.0)).xyz;
    gl_Position = P * V * M * vec4(v_position, 1.0);
    /*if ( v_uv.x==0.0 && v_uv.y==0.0 ) {
        gl_Position = P * V * M * vec4(2.0, v_position.y, v_position.z, 1.0);
    } else {
        gl_Position = P * V * M * vec4(v_position, 1.0);
    }*/
    texCoord = v_uv.xy;//get rid of extra for now

    //shading
    //debug
    vec3 N = normalize(V * M * vec4(v_normal.xyz, 0.0)).xyz;
    vec3 L = normalize(LightPosition.xyz - ((V * M * vec4(v_position, 1.0)).xyz));
    vec3 E = normalize(-((V * M * vec4(v_position, 1.0)).xyz));
    vec3 H = normalize(L+E);

    diffuse = max(dot(L, N), 0.0) * DiffuseProduct;
    //diffuse = max(dot(L, N), 0.0);
    //specular = max( max( dot(N, H), 0.0 ) * SpecularProduct, 0.0 ) ;
    specular = max(pow( max( dot(N, H), 0.0 ), Shininess ) * SpecularProduct, 0.0);
    //ambient = vec4(0.2,0.2,0.2,1.0);
    ambient = AmbientProduct;

    vec4 what = vec4((ambient + diffuse + specular).xyz, 1.0);
    color = what;
}
