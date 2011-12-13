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
//parallax-mapping(inspirace): http://www.dhpoware.com/demos/glslParallaxNormalMapping.html

#define MAX_LIGHTS 4 //maximalni pocet svetel

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


///////TEXTURY//////
uniform sampler2D texture1; //color map
uniform sampler2D texture2; //normal map
uniform sampler2D texture3; //height map

uniform bool useTexture; //jsou-li samplery naplnene, tak vraci true


//texture coordinates
in vec2 t;

out vec4 fragColor; //vystupni barva

void main() {
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);
	if(useTexture) {
		//nove koordinaty po posunuti v parallax mappingu
		vec2 newTexCoord;

		//konstanty ovlivnujici plasticnost parallax mappingu
		float scale = 0.03f;
		float bias =  0.00;

		//zvyseni opakovani textury
		float scaleCoord = 5;

		////////////////////////PARALLAX MAPPING///////////////////////////////
		//spocitame offset posunuti
		float height = texture(texture3, t  * scaleCoord).r;
		height = height * scale + bias;
		newTexCoord = (t +  ( normalize(tanCam).xy * height)) * scaleCoord;


		//////////////////////////////OSVETLENI////////////////////////////////
		//Nastaveni fyzikalnich konstant pro slabnuti svetla se vzdalenosti
		float attenuation, distance, radius; 
		radius = 1.0; //ovlivnuje jak daleko svetlo dosviti
		float constantAtt = 1.0; 
		vec4 diffuseF;

		//pocitame osvetleni pr ovsechna svetla
		for(int i = 0; i < enabledLights; i++) { 
			//rozhoduje zda-li se bude vykreslovat ambientni svetlo
			if(paintAmbient)
				finalColor += material.ambient * lights[i * 3 + 2]; 
		
			distance = length(tanLightDir[i] / radius);	
			//slabnuti svetla
			attenuation = 1.0 / (constantAtt + LINEAR_ATTENUATION * distance +
											   QUADR_ATTENUATION * distance * distance);
									   
			//spocitame difuzni slozku	
			vec3 nMap = normalize(texture(texture2, newTexCoord).rgb * 2.0 - 1.0); //dostavame do rozasahu -1, 1
					   	
			vec3 N = normalize(nMap); //normala plosky dle textury
			vec3 L = normalize(tanLightDir[i]); //paprsek svetla v tangentovem prostoru

			//urcuje, zda-li se vykresli difuzni slozka spektra
			if(paintDiffSpec) {
				float diffuse = max(dot(N,L),0.0);
				diffuseF =  lights[i * 3 + 1]; // * material.diffuse;
				finalColor += attenuation * diffuse * diffuseF;
			}
		}
		fragColor = texture(texture1, newTexCoord) * finalColor;
	} else {
		fragColor = finalColor; //cerna barva znaci error pri nacitani textur
	}

}
