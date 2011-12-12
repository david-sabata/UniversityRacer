#version 130

//Studium osvetlovaciho modelu
//potrebne zdroje:
//point-light(studium): http://www.lighthouse3d.com/tutorials/glsl-tutorial/point-light-per-pixel/
//blinn-phong(studium): http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
//parallax-mapping(inspirace): http://www.dhpoware.com/demos/glslParallaxNormalMapping.html

#define MAX_LIGHTS 4 //maximalni pocet svetel

//zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
#define LINEAR_ATTENUATION 0.022
#define QUADR_ATTENUATION 0.0019 

// nacitani textur
// @LOAD materials/textures/carpet2.bmp
uniform sampler2D tex;

// @LOAD materials/textures/carpet2_norm.bmp
uniform sampler2D texNormal;

// @LOAD materials/textures/carpet2_disp.bmp
uniform sampler2D texHeight;

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

void main() {
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);
	
	//nove koordinaty po posunuti v parallax mappingu
	vec2 newTexCoord;

	//konstanty ovlivnujici plasticnost parallax mappingu
	float scale = 0.03f;
	float bias =  0.00;

	//zvyseni opakovani textury
	float scaleCoord = 5;

	////////////////////////PARALLAX MAPPING///////////////////////////////
	//spocitame offset posunuti
	float height = texture(texHeight, t  * scaleCoord).r;
	height = height * scale + bias;
	newTexCoord = (t +  ( normalize(tanCam).xy * height)) * scaleCoord;


	//////////////////////////////OSVETLENI////////////////////////////////
	//Nastaveni fyzikalnich konstant pro slabnuti svetla se vzdalenosti
	float attenuation, distance, radius; 
	radius = 1.0; //ovlivnuje jak daleko svetlo dosviti
	
	//pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
	float constantAtt = 1.0;
	float linearAtt = LINEAR_ATTENUATION;
	float quadraticAtt = QUADR_ATTENUATION;
	vec4 diffuseF;

	//pocitame osvetleni pr ovsechna svetla
	for(int i = 0; i < enabledLights; i++) { 
		//rozhoduje zda-li se bude vykreslovat ambientni svetlo
		if(paintAmbient)
			finalColor += material.ambient * lights[i * 3 + 2]; 
		
		distance = length(tanLightDir[i] / radius);	
		//slabnuti svetla
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
		//spocitame difuzni slozku	
		vec3 nMap = normalize(texture(texNormal, newTexCoord).rgb * 2.0 - 1.0); //dostavame do rozasahu -1, 1
					   	
		vec3 N = normalize(nMap); //normala plosky dle textury
		vec3 L = normalize(tanLightDir[i]); //paprsek svetla v tangentovem prostoru

		//urcuje, zda-li se vykresli difuzni slozka spektra
		if(paintDiffSpec) {
			float diffuse = max(dot(N,L),0.0);
			diffuseF =  lights[i * 3 + 1]; // * material.diffuse;
			finalColor += attenuation * diffuse * diffuseF;
		}
	}
	gl_FragColor = texture(tex, newTexCoord) * finalColor;
}
