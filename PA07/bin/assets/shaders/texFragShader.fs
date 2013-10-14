varying vec4 color;
varying vec2 texCoord;

//out vec4 fColor;

uniform sampler2D texture;

void main(void){
	//gl_FragColor = color + texture2D( texture, texCoord);
    if (texture2D( texture, texCoord).r ==0.0 &&  texture2D( texture, texCoord).g ==0.0 && texture2D( texture, texCoord).b ==0.0) {
	if (texture2D(texture, texCoord).a ==0.0) {
    	    gl_FragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	} else {
            gl_FragColor = vec4(texture2D(texture, texCoord).aaa, 1.0);
        }
    } else {
    	gl_FragColor = vec4(texture2D(texture, texCoord).x, color.g, texture2D(texture, texCoord).z, 1.0);
    }
}
