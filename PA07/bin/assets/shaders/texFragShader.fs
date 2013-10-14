varying vec4 color;
varying vec2 texCoord;

//out vec4 fColor;

uniform sampler2D texture;

void main(void){
	//gl_FragColor = color * texture2D( texture, texCoord);
    gl_FragColor = (color + texture2D( texture, texCoord ));
}
