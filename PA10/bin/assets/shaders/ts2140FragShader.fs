/*
Texturing and Shading fragment shader. Shading in matching vertex shader. version 1.40.
*/

//#version 330 ///I wish. versions are: 1.00 ES, 1.10, 1.20, and 1.30 on laptop. BAD laptop.

//varying vec4 color;
varying vec2 texCoord;
varying vec4 position, N, E;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct Light {
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    vec4 spotDirection;
};

uniform sampler2D textSamp;
uniform Light lights[8];
uniform Material material;

void main(void){
    vec4 color[8];
    vec4 ambient, diffuse, specular;
    vec4 runColor = vec4(0.0,0.0,0.0,1.0);
    vec4 lightDirection;
    //shading
    vec4 H;
    float attenuation=1.0;

    for (int i=0; i<8; i++) {
        //set parameters for point spot and distant light...
        if(0.0 == lights[i].position.w) { //did I swap this?
            attenuation = 1.0; // no attenuation
            lightDirection = normalize(lights[i].position);
        }
        else {
            //direction of light to object
            float distance = length(vec3(lights[i].position - position));
            lightDirection = normalize((lights[i].position - position));
            attenuation = 1.0 / (lights[i].constantAttenuation
                                   + lights[i].linearAttenuation * distance
                                   + lights[i].quadraticAttenuation * distance * distance);
            if (lights[i].spotCutoff <= 90.0) // spotlight?
            {
                float clampedCosine = max(0.0, dot(-lightDirection, normalize(lights[i].spotDirection)));
                if (clampedCosine < cos(radians(lights[i].spotCutoff))) // outside of spotlight cone?
                {
                    attenuation = 0.0;
                }
                else
                {
                    attenuation = attenuation * pow(clampedCosine, lights[i].spotExponent);   
                }
            }
        }

        //Set diffuse
        diffuse = lights[i].diffuse * material.diffuse * max(0.0, dot(lightDirection,N))* attenuation;
        //set specular
        H = normalize(lightDirection+E);
        if (dot(N,H) < 0.0) {
            specular = vec4(0.0,0.0,0.0,0.0);
        } else {
            specular = lights[i].specular * material.specular * 
                pow( max(0.0, dot(N,H)), material.shininess) * attenuation;

        }
        //ambient
        ambient = (material.ambient * lights[i].ambient);
        
        color[i] = vec4((ambient + diffuse + specular).xyz, 1.0);
    }

    //calculate lighting for color before
    for (int i=0; i<8; i++) {
        runColor = (0.5*color[i]) + runColor;
    }

    gl_FragColor = runColor * texture2D(textSamp, texCoord);
}
