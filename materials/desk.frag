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

//vlastnosti materiulu
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
in vec3 eyeLightPos[MAX_LIGHTS]; // pozice svetla v prostoru OKA

in vec2 t;
in vec3 oPosition;

out vec4 fragColor; //vystupni barva

////////////////////GENEROVANI SUMU////////////////////////
//SMEROVE VEKTORY
//nastudovano zde - http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
const vec3 grad3D[] = vec3[](vec3(1,1,0), vec3(-1,1,0), vec3(1,-1,0), vec3(-1,-1,0), 
				  vec3(1,0,1),vec3(-1,0,1), vec3(1,0,-1), vec3(-1,0,-1), 
				  vec3(0,1,1), vec3(0,-1,1), vec3(0,1,-1), vec3(0,-1,-1));

//Nahodne cislo, dle ktereho se pak vybere gradient
vec4 p[] = vec4[](vec4(151,160,137,91),	vec4(90,15,131,13),		vec4(201,95,96,53),
			vec4(194,233,7,225),	vec4(140,36,103,30),	vec4(69,142,8,99),
			vec4(37,240,21,10),		vec4(23,190, 6,148),	vec4(247,120,234,75),
			vec4(0,26,197,62),		vec4(94,252,219,203),	vec4(117,35,11,32),
			vec4(57,177,33,88),		vec4(237,149,56,87),	vec4(174,20,125,136),
			vec4(171,168, 68,175),	vec4(74,165,71,134),	vec4(139,48,27,166),
			vec4(77,146,158,231),	vec4(83,111,229,122),	vec4(60,211,133,230),
			vec4(220,105,92,41),	vec4(55,46,245,40),		vec4(244,102,143,54),
			vec4(65,25,63,161),		vec4(1,216,80,73),		vec4(209,76,132,187),
			vec4(208, 89,18,169),	vec4(200,196,135,130),	vec4(116,188,159,86),
			vec4(164,100,109,198),	vec4(173,186, 3,64),	vec4(52,217,226,250),
			vec4(124,123,5,202),	vec4(38,147,118,126),	vec4(255,82,85,212),
			vec4(207,206,59,227),	vec4(47,16,58,17),		vec4(182,189,28,42),
			vec4(223,183,170,213),	vec4(119,248,152, 2),	vec4(44,154,163, 70),
			vec4(221,153,101,155),	vec4(167, 43,172,9),	vec4(129,22,39,253),
			vec4(19,98,108,110),	vec4(79,113,224,232),	vec4(178,185, 112,104),
			vec4(218,246,97,228),	vec4(251,34,242,193),	vec4(238,210,144,12),
			vec4(191,179,162,241),	vec4(81,51,145,235),	vec4(249,14,239,107),
			vec4(49,192,214, 31),	vec4(181,199,106,157),	vec4(184, 84,204,176),
			vec4(115,121,50,45),	vec4(127, 4,150,254),	vec4(138,236,205,93),
			vec4(222,114,67,29),	vec4(24,72,243,141),	vec4(128,195,78,66),
			vec4(215,61,156,180));

//Pomocna fce pro vyhledavani konkretnich prvku v poli vektoru velikosti 4
int gIG(float index, float size, float gradSize) {
	index = mod(index,size);
	int mainIndex = int(index * 0.25);
	int smallIndex = int(mod(index, 4.0));

	return int(mod(((p[mainIndex])[smallIndex]),gradSize));
}

//funkce f(t) = 6t^5 - 15t^4 + 10t^3
float f(float t) {
	return (t * t * t * ( t * t * 6 - t * 15 + 10));
}


float noise3D(float Px,float Py, float Pz) {

	//najdeme nejblizzsi integer cislo
	float i = floor(Px);
	float j = floor(Py);
	float k = floor(Pz);

	float size = 253.0;
	float size2 = 12.0;


	/////////////////NOVY/////////////////////
	//zjistime indexy do tabulky gradientu (kazdemu rohu krychle priradime index)	
	int g000 = gIG(i + gIG(j + gIG(k,size,size2),size,size2),size, size2);
	int g001 = gIG(i + gIG(j + gIG(k + 1,size,size2),size,size2),size, size2);
	int g010 = gIG(i + gIG(j + 1 + gIG(k,size,size2),size,size2),size, size2);
	int g011 = gIG(i + gIG(j + 1 + gIG(k + 1,size,size2),size,size2),size, size2);
	int g100 = gIG(i + 1 + gIG(j + gIG(k,size,size2),size,size2),size, size2);
	int g101 = gIG(i + 1 + gIG(j + gIG(k + 1,size,size2),size,size2),size, size2);
	int g110 = gIG(i + 1 + gIG(j + 1 + gIG(k,size,size2),size,size2),size, size2);
	int g111 = gIG(i + 1 + gIG(j + 1 + gIG(k + 1,size,size2),size,size2),size, size2);
	
	
	//spocitame relativni souradnice posunuti rohu ohledne hledaneho bodu
	float u = Px - i;
	float v = Py - j;
	float w = Pz - k;


	//secteme priruste pro kazdy z 8mi rohu krychle
	float n000 = dot(grad3D[g000],vec3(u,v,w));
	float n001 = dot(grad3D[g001],vec3(u,v,w - 1.0));
	float n010 = dot(grad3D[g010],vec3(u,v - 1.0,w));
	float n011 = dot(grad3D[g011],vec3(u,v - 1.0,w - 1.0));
	float n100 = dot(grad3D[g100],vec3(u - 1.0,v,w));
	float n101 = dot(grad3D[g101],vec3(u - 1.0,v,w - 1.0));
	float n110 = dot(grad3D[g110],vec3(u - 1.0,v - 1.0,w));
	float n111 = dot(grad3D[g111],vec3(u - 1.0,v - 1,w - 1.0));


	float f_u = f(u);
	float f_v = f(v);
	float f_w = f(w);

	//interpolace podel x
	float nx00 = mix(n000, n100, f_u);
	float nx01 = mix(n001, n101, f_u);
	float nx10 = mix(n010, n110, f_u);
	float nx11 = mix(n011, n111, f_u);

	//interpolace podel y

	float nxy0 = mix(nx00, nx10, f_v);
	float nxy1 = mix(nx01, nx11, f_v);

	//interpolace podel z
	float nxyz = mix(nxy0, nxy1, f_w);

	return nxyz;
}


void main() {
	vec3 lightDir;
	float radius = 1.0; //pak se nasobi dale - optimalizace, proto je opacny

	//kdyz je vse zhasnute, bude tma
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);
	
	float scale = 19.0;
	//generovani nahodnych uprav normal
	float intensity = (noise3D(oPosition.x * scale,oPosition.y * scale,oPosition.z * scale) * 0.0526315789473);
	vec3 modEyeNormal = eyeNormal + intensity;

	vec3 N = normalize(modEyeNormal);

	//vypocet half vectoru (HV)
	//v eyespace muzeme povazovat za vektor pozorovatele eyePosition, jeho otocenim tak ziskame 
	//vektor z plosky do pozorovaele
	vec3 V = normalize(-eyePosition);

	//////////////////////////////////////SVETLA/////////////////////////////////////
	float constantAtt = 1.0; //konstanta pro ubytek svetla
	for(int i = 0; i < enabledLights ; i++) {
		
		//zda-li se bude vykreslovat ambientni slozka svetla
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2];

		//vektor paprsku svetla
		lightDir = eyeLightPos[i] - eyePosition;

		//slabnuti svetla
		float attenuation, distance;
		distance = length(lightDir * radius);	 
		//pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
		attenuation = 1.0 / (constantAtt + LINEAR_ATTENUATION * distance +
										   QUADR_ATTENUATION * distance * distance);
	
		//zda-li se bude kreslit difuzni a specularni slozka svetla
		if(paintDiffSpec) {
			//normalizovany vektor svetla							  
			vec3 L = normalize(lightDir);
			
			//difuzni slozka
			float diffuse = max(dot(N,L),0.0);
			vec4 diffuseF = material.diffuse * lights[i * 3 + 1];
			vec4 diff = attenuation * diffuse * diffuseF;
	
			//vypocet halfvector = L + V - mezi light a pozorovatelem
			vec3 H = normalize(L + V);
	
			//spocitame spekularni odlesk
			float specular = pow(dot(N,H), material.shininess);
	
			vec4 spec = vec4(0.0,0.0,0.0,1.0);
			//pricteme spekulární složku k výsledné barvì
			if(specular >= 0.0)
				spec = attenuation *  specular * material.specular;
			finalColor +=  diff +  spec;
		}
	} 	
	fragColor = finalColor;
}
 