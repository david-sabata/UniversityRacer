#version 130

//Studium osvetlovaciho modelu
//potrebne zdroje:
//point-light(studium): http://www.lighthouse3d.com/tutorials/glsl-tutorial/point-light-per-pixel/
//blinn-phong(studium): http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php

#define MAX_LIGHTS 4

//zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
#define LINEAR_ATTENUATION 0.022
#define QUADR_ATTENUATION 0.0019 


// @LOAD materials/textures/ceiling.bmp
uniform sampler2D tex;

// @LOAD materials/textures/ceiling_bump.bmp
uniform sampler2D texNormal;


struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform Material material;

//umoznuji vybrat ktere svetlo se bude kreslit
uniform bool paintDiffSpec;
uniform bool paintAmbient;

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

//tangent space
in vec3 tanCam;
in vec3 tanLightDir[MAX_LIGHTS];

//texture coordinates
in vec2 t;

//debug color
in vec4 color;

void main() {
	
	
	vec2 newTexCoord;

	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);

	float scaleCoord = 5;

	//////////////////////////////OSVETLENI////////////////////////////////
	//Nastaveni fyzikalnich konstant pro slabnuti svetla se vzdalenosti
	float attenuation, distance, radius; 
	radius = 1.0;
	

	float constantAtt = 1.0;
	float linearAtt = LINEAR_ATTENUATION ;
	float quadraticAtt = QUADR_ATTENUATION;
	vec4 diffuseF;
	for(int i = 0; i < enabledLights; i++) { 
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2]; 
		distance = length(tanLightDir[i] / radius);	
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
		//spocitame odraz (difuzni slozku)	
		vec3 nMap = normalize(texture(texNormal, t * scaleCoord).rgb * 2.0 - 1.0);
					   	
		vec3 N = normalize(nMap); //normala plosky dle textury
		vec3 L = normalize(tanLightDir[i]); //paprsek svetla v tangentovem prostoru

		if(paintDiffSpec) {
			//difuzni slozka
			float diffuse = max(dot(N,L),0.0);
		
			diffuseF =  material.diffuse * lights[i * 3 + 1]; // * material.diffuse;
			finalColor += attenuation * diffuse * diffuseF;
		}
	}
		
	gl_FragColor = texture(tex, t * scaleCoord) * finalColor;
}
