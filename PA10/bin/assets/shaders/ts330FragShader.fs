/*
Texturing and Shading fragment shader. Shading in matching vertex shader. version 3.30 for practice and comparison.
*/

#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

in vec4 color;
in vec2 texCoord;

out vec4 fColor;

uniform sampler2D textSamp;

void main(void){
	//gl_FragColor = color + texture2D( textSamp, texCoord);
    fColor = color * texture(textSamp, texCoord);
}
