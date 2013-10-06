attribute vec3 v_position;
attribute vec3 v_normal;
varying vec4 color;

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
