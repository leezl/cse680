attribute vec3 v_position;
//attribute vec4 v_color;
attribute vec3 v_normal;
//attribute vec2 v_uv;

varying vec4 color;

//need following for shading I believe.
uniform mat4 M;
uniform mat4 V;
unifrom mat4 P;
uniform vec4 LightPosition;
uniform vec4 DiffuseProduct, SpecularProduct, AmbientProduct;
uniform float Shininess;

void main(void){
    //positioning
    gl_Position = P * V * M * vec4(v_position, 1.0);

    //shading
    vec4 diffuse, specular;
    vec3 N = normalize(v_normal.xyz);
    vec3 L = normalize(LightPosition.xyz - (V * M * vec4(v_position,1.0)).xyz).xyz;
    diffuse = max(dot(L,N), 0.0)*DiffuseProduct;

    vec3 E = -normalize((V * M * vec4(v_position,1.0)).xyz);
    vec3 H = normalize(L+E);
    specular = max(pow(max(dot(N, H), 0.0), Shininess)*SpecularProduct, 0.0);

    color = vec4((AmbientProduct + diffuse + specular).xyz, 1.0);
}
