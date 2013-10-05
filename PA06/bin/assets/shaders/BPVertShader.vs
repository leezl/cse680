attribute vec4 v_position;
attribute vec4 v_normal;
varying vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec4 LightPosition;
uniform vec4 DiffuseProduct, SpecularProduct, AmbientProduct;
uniform float Shininess;

void main(void){
    vec4 diffuse, specular, ambient;

    //positioning
    vec3 pos = (V * M * v_position).xyz;
    gl_Position = P * V * M * v_position;

    //shading
    //debug
    //vec3 N = normalize(V * M * v_normal).xyz;
    //vec3 N = normalize(v_normal).xyz;
    vec3 N =vec3(0.0, 1.0, 0.0);
    vec3 L = normalize(LightPosition.xyz - pos);
    vec3 E = normalize(-pos);
    vec3 H = normalize(L+E);

    diffuse = max(dot(L, N), 0.0) * DiffuseProduct;
    specular = pow( max( dot(N, H), 0.0 ), Shininess ) * SpecularProduct;
    ambient = AmbientProduct;
    if ( dot(L, N) < 0.0 ) {
        specular = vec4(v_normal.xyz, 1.0);
        diffuse = vec4(v_normal.xyz, 1.0);
        ambient = vec4(v_normal.xyz,1.0);
    }
    vec4 what = vec4((ambient + diffuse + specular).xyz, 1.0);
    color = what;
}
