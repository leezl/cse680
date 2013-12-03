/*
Texturing and Shading fragment shader. Shading in matching vertex shader. version 1.40.
*/

//#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

varying vec2 texCoord;
//varying vec4 brightness;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

//out vec4 fColor;

uniform sampler2D textSamp;

void main(void){
    //should totally pass this in, but only need it for skybox...
    vec4 brightness = vec4(0.6,0.6,0.6, 0.0);

	gl_FragColor = brightness * texture2D( textSamp, texCoord);
    //fColor = color * texture(textSamp, texCoord);
}
