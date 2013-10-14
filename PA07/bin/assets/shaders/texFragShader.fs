#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

in vec4 color;
in vec2 texCoord;

out vec4 fColor;

uniform sampler2D textSamp;

void main(void){
	//gl_FragColor = color + texture2D( textSamp, texCoord);
    fColor = color + texture(textSamp, texCoord);
    /*if (texture2D( textSamp, texCoord).r ==0.0 &&  texture2D( textSamp, texCoord).g ==0.0 && texture2D( textSamp, texCoord).b ==0.0) {
    	if (texture2D(textSamp, texCoord).a ==0.0) {
        	    //gl_FragColor = vec4(texCoord.x, 0.0, 0.0, 1.0);
                fColor = vec4(texCoord.x, 0.0, 0.0, 1.0);
    	} else {
            //gl_FragColor = vec4(texture2D(textSamp, texCoord).aaa, 1.0);
            fColor = vec4(texture2D(textSamp, texCoord).aaa, 1.0);
        }
    } else {
    	//gl_FragColor = vec4(0.0, color.g, 0.0, 1.0);
        fColor = vec4(0.0, color.g, 0.0, 1.0);
    }*/
    /*if (texCoord[0]>0.5 && texCoord[1]>0.5) {
        fColor = color + texture2D(textSamp, vec2(1.0,1.0));
    } else if (texCoord[0]<0.5 && texCoord[1]>0.5) {
        fColor = color + texture2D(textSamp, vec2(0.0,1.0));
    } else if (texCoord[0]>0.5 && texCoord[1]<0.5) {
        fColor = color + texture2D(textSamp, vec2(1.0,0.0));
    } else //if (texCoord[0]<0.5 && texCoord[1]<0.5) 
    {
        fColor = color +texture2D(textSamp, vec2(0.0,0.0));
    }*/
}
