/*
Texturing and Shading fragment shader. Shading in matching vertex shader. version 1.40.
*/

//#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

varying vec4 color;
varying vec2 texCoord;

//out vec4 fColor;

uniform sampler2D textSamp;

void main(void){
	gl_FragColor = color + texture2D( textSamp, texCoord);
    //fColor = color * texture(textSamp, texCoord);
}
