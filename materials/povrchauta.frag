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

//vlastnosti materialu
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
in vec3 eyeLightPos[MAX_LIGHTS]; // pozice svetel v prostoru OKA

uniform sampler2D texture1;
uniform bool useTexture;

in vec2 t; // texturovaci souradnice

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
	float constantAtt = 1.0;
	for(int i = 0; i < enabledLights ; i++) {

		// urcuje zda-li se vykreslia mbientni slozka svetla
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2];

		//vektor od zdoje svetla na vykreslovanou plosku
		lightDir = eyeLightPos[i] - eyePosition;

		//slabnuti svetla v zavislosti na vzdalenosti od zdroje
		float attenuation, distance;
		distance = length(lightDir / radius);	 
		attenuation = 1.0 / (constantAtt + LINEAR_ATTENUATION * distance +
										   QUADR_ATTENUATION * distance * distance);
							

		//urcuje, zda-li se vyrkesli difuzni a spekularni slozka
		if(paintDiffSpec) {
			//normalizovany vektor od svetla		  
			vec3 L = normalize(lightDir);

			//difuzni slozka
			float diffuse = max(dot(N,L),0.0);
			vec4 diffuseF = 	material.diffuse * lights[i * 3 + 1];
			vec4 diff = attenuation * diffuse * diffuseF;
	
			//halfvector = L + V - mezi light a pozorovatelem
			vec3 H = normalize(L + V);
	
			//spocitame spekularni odlesk
			float specular = pow(dot(N,H), material.shininess);
	
			vec4 spec = vec4(0.0,0.0,0.0,1.0);
			//pricteme spekulární složku k výsledné barvi
			if(specular >= 0.0)
				spec = attenuation *  specular * material.specular;
			finalColor += diff +  spec;
			if(useTexture) {
				finalColor = (texture(texture1,t) / MAX_LIGHTS) + finalColor;
			}
		}
	} 
	fragColor = finalColor;
}
