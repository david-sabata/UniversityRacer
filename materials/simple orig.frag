#version 130

#define MAX_LIGHT 4
#define LINEAR_ATTENUATION 0.006
#define QUADR_ATTENUATION 0.0001 


uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

in vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
in vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
in vec3 eyeLightPos[MAX_LIGHT];
in vec3 lightDir[MAX_LIGHT];

//potreba pro spekularni odlesky
varying vec3 halfVector;


//import vlastnosti materialu
in vec4 ambientF[MAX_LIGHT];
in vec4 diffuseF[MAX_LIGHT];
in vec4 specularF;
flat in int shininessF;


in vec4 color;

void main() {
	
	//kdyz je vse zhasnute, bude tma
	vec4 finalColor = vec4(0.0,0.0,0.0,1.0);

	vec3 N = normalize(eyeNormal);

	//vypocet half vectoru (HV)
	//v eyespace muzeme povazovat za vektor pozorovatele eyePosition, jeho otocenim tak ziskame 
	//vektor z plosky do pozorovaele
	vec3 V = normalize(-eyePosition);



		//ambientni slozka
		finalColor += ambientF[0]; //ambientF obsahuje i ambientni slozku svetla
	
		//////////////////////////////////////SVETLO 1/////////////////////////////////////
		//slabnuti svetla
		float attenuation, distance;
		distance = length(lightDir[0]);	 
		 //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
		float constantAtt = 1.0;
		float linearAtt = LINEAR_ATTENUATION;
		float quadraticAtt = QUADR_ATTENUATION;
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
									   	
		
		vec3 L = normalize(lightDir[0]);

	
		//difuzni slozka
		float diffuse = max(dot(N,L),0.0);
		vec4 diff = attenuation * diffuse * diffuseF[0];
	
		//halfvector = L + V - mezi light a pozorovatelem
		vec3 H = normalize(L + V);
	
		//spocitame spekularni odlesk
		float specular = pow(dot(N,H), 64);
	
		//pricteme spekulární složku k výsledné barvì
		if(specular >= 0.0)
			vec4 spec = attenuation * specularF * specular;
	    finalColor +=  diff + spec;
	 
		///////////////////////////////////////SVETLO 2/////////////////////////////////////////

		//slabnuti svetla
		
		distance = length(lightDir[1]);	 
		 //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
									   	
		
		L = normalize(lightDir[1]);

	
		//difuzni slozka
		diffuse = max(dot(N,L),0.0);
		diff = attenuation * diffuse * diffuseF[1];
	
		//halfvector = L + V - mezi light a pozorovatelem
		H = normalize(L + V);
	
		//spocitame spekularni odlesk
	    specular = pow(dot(N,H), 64);
	
		//pricteme spekulární složku k výsledné barvì
		if(specular >= 0.0)
			spec = attenuation * specularF * specular;
	    finalColor +=  diff + spec;

		///////////////////////////////////////SVETLO 3/////////////////////////////////////////

		//slabnuti svetla
		
		distance = length(lightDir[2]);	 
		 //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
									   	
		
		L = normalize(lightDir[2]);

	
		//difuzni slozka
		diffuse = max(dot(N,L),0.0);
		diff = attenuation * diffuse * diffuseF[2];
	
		//halfvector = L + V - mezi light a pozorovatelem
		H = normalize(L + V);
	
		//spocitame spekularni odlesk
	    specular = pow(dot(N,H), 64);
	
		//pricteme spekulární složku k výsledné barvì
		if(specular >= 0.0)
			spec = attenuation * specularF * specular;
	    finalColor +=  diff + spec;
		
		///////////////////////////////////////SVETLO 4/////////////////////////////////////////

		//slabnuti svetla
		
		distance = length(lightDir[3]);	 
		 //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
									   	
		
		L = normalize(lightDir[3]);

	
		//difuzni slozka
		diffuse = max(dot(N,L),0.0);
		diff = attenuation * diffuse * diffuseF[3];
	
		//halfvector = L + V - mezi light a pozorovatelem
		H = normalize(L + V);
	
		//spocitame spekularni odlesk
	    specular = pow(dot(N,H), 64);
	
		//pricteme spekulární složku k výsledné barvì
		if(specular >= 0.0)
			spec = attenuation * specularF * specular;
	    finalColor +=  diff + spec;



	//gl_FragColor = texture2D(textureNormal,t);
	//gl_FragColor = ambientF[2];
	//gl_FragColor = vec4(diffuse,0.0,0.0,1.0);		
	gl_FragColor = finalColor;
}
