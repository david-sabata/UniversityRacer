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

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

//tangent space
in vec3 tanCam;
in vec3 tanLightDir[MAX_LIGHTS];

uniform sampler2D texture1; //color map
uniform sampler2D texture2;// normal map

//indikuje zda-li se spravne nacetli textury
uniform bool useTexture;

//texturovaci souradnice
in vec2 t; 

out vec4 fragColor; //vystupni barva

void main() {
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);
	float scaleCoord = 5;

	//////////////////////////////OSVETLENI////////////////////////////////
	/////SLABNUTI SVETLA////
	//Nastaveni fyzikalnich konstant pro slabnuti svetla se vzdalenosti
	float attenuation, distance, radius; 
	radius = 1.0;
	float constantAtt = 1.0;//konstanta pro slabnuti svetla
	
	for(int i = 0; i < enabledLights; i++) { 
		//zda-li se vykresli ambientni slozka svetla
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2]; 
		
		//ubytek svetla se vzdalenosti od zdroje
		distance = length(tanLightDir[i] / radius);	
		attenuation = 1.0 / (constantAtt + LINEAR_ATTENUATION * distance +
										   QUADR_ATTENUATION * distance * distance);
									   
		//spocitame odraz (difuzni slozku)	
		///jednoduchy bump mapping - pocita se na urovni tangent space
		vec3 nMap = normalize(texture(texture2, t * scaleCoord).rgb * 2.0 - 1.0); //dostanema do razsahu -1, 1			   	
		vec3 N = normalize(nMap); //normala plosky dle textury
		vec3 L = normalize(tanLightDir[i]); //paprsek svetla v tangentovem prostoru

		//zda-li se vykresluje difuzni a spekularni slozka svetla
		if(paintDiffSpec) {
			//difuzni slozka
			float diffuse = max(dot(N,L),0.0);
			vec4 diffuseF =  material.diffuse * lights[i * 3 + 1]; // * material.diffuse;
			finalColor += attenuation * diffuse * diffuseF;
		}
	}
	if(useTexture)
		fragColor = texture(texture1, t * scaleCoord) * finalColor;
	else 
		fragColor = finalColor;
}
