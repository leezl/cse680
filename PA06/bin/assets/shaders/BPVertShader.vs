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
    vec3 N = normalize(V * M * v_normal).xyz;
    vec3 L = normalize(LightPosition.xyz - pos);
    vec3 E = normalize(-pos);
    vec3 H = normalize(L+E);

    diffuse = max(dot(L, N), 0.0) * DiffuseProduct;
    specular = pow( max( dot(N, H), 0.0 ), Shininess ) * SpecularProduct;
    if ( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    }
    ambient = AmbientProduct;
    color = vec4((ambient + diffuse + specular).xyz, 1.0);
}
