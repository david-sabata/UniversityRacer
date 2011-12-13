// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#version 130

//Studium osvetlovaciho modelu
//potrebne zdroje:
//point-light(studium): http://www.lighthouse3d.com/tutorials/glsl-tutorial/point-light-per-pixel/
//blinn-phong(studium): http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php

#define MAX_LIGHTS 4

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

//zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
#define LINEAR_ATTENUATION 0.022
#define QUADR_ATTENUATION 0.0019 

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

in vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
in vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
in vec3 eyeLightPos[MAX_LIGHTS]; //pozice svetla v prostoru OKA

uniform sampler2D texture1;
uniform bool useTexture;

in vec2 t; //texturovaci souradnice

out vec4 fragColor; //vystupni barva

void main() {
	vec3 lightDir;
	float radius = 1.0;

	//kdyz je vse zhasnute, bude tma
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);

	vec3 N = normalize(eyeNormal);

	//v eyespace muzeme povazovat za vektor pozorovatele eyePosition, jeho otocenim tak ziskame 
	//vektor z plosky do pozorovaele
	vec3 V = normalize(-eyePosition);

	//////////////////////////////////////SVETLA/////////////////////////////////////
	for(int i = 0; i < enabledLights ; i++) {
		//zda-li se vykresli ambientni slozka svetla
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2];

		lightDir = eyeLightPos[i] - eyePosition;

		//slabnuti svetla
		float attenuation, distance;
		distance = length(lightDir / radius);	 

		float constantAtt = 1.0;
		float linearAtt = LINEAR_ATTENUATION;
		float quadraticAtt = QUADR_ATTENUATION;
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									  
		//zda-li sevykresli specularn ia difuzni slozka svetla
		if(paintDiffSpec) {
			//normalizovany vektor svetla
			vec3 L = normalize(lightDir);

			//difuzni slozka
			float diffuse = max(dot(N,L),0.0);
			vec4 diffuseF = material.diffuse * lights[i * 3 + 1];
			vec4 diff = attenuation * diffuse * diffuseF;
		
			//halfvector = L + V - mezi light a pozorovatelem
			vec3 H = normalize(L + V);
	
			//spocitame spekularni odlesk
			float specular = pow(dot(N,H), material.shininess);
	
			vec4 spec = vec4(0.0,0.0,0.0,1.0);
			//pricteme spekulární složku k výsledné barvi
			if(specular >= 0.0)
				spec = attenuation *  specular * material.specular;
			finalColor +=  diff +  spec;
		}
	} 

		//zajisti, ze hodiny sviti i ve tme
		if(useTexture) {
			vec4 texel = texture(texture1, t);
			vec4 color = finalColor * texture(texture1, t);
			color = max(color,texel);
			fragColor = color;
		} else {
			fragColor = finalColor; //pokud neexistuje textura hodin, pak je vykresli standardne
		}
}
