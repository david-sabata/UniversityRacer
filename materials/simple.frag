#version 130


in vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
in vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
in vec3 eyeLightPos;
in vec3 lightDir;

//potreba pro spekularni odlesky
varying vec3 halfVector;


//import vlastnosti materialu
in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
flat in int shininessF;



in vec4 color;

void main() {
	
	
	//ambientni slozka
	vec4 finalColor = ambientF + vec4(0.1,0.1,0.1,1.0); //ambientF obsahuje i ambientni slozku svetla

	//slabnuti svetla
	float attenuation, distance;
	distance = length(lightDir);	 
	 //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
    float constantAtt = 1.0;
	float linearAtt = 0.0014;
	float quadraticAtt = 0.000007;
	attenuation = 1.0 / (constantAtt + linearAtt * distance +
									   quadraticAtt * distance * distance);
									   
									   	
	vec3 N = normalize(eyeNormal);
	vec3 L = normalize(lightDir);

	
	//difuzni slozka
	float diffuse = max(dot(N,L),0.0);
	diffuse = clamp(diffuse,0.0,1.0);
	finalColor += attenuation * diffuse * diffuseF;
	
	
	//vypocet half vectoru (HV)
	//v eyespace muzeme povazovat za vektor pozorovatele eyePosition, jeho otocenim tak ziskame 
	//vektor z plosky do pozorovaele
	vec3 V = normalize(-eyePosition);
	
	//halfvector = L + V - mezi light a pozorovatelem
	vec3 H = normalize(L + V);
	
	//spocitame spekularni odlesk
	float specular = pow(dot(N,H), 64);
	
	//pricteme spekulární složku k výsledné barvì
	if(specular >= 0.0)
		finalColor += attenuation * specularF * specular;
		  

	//gl_FragColor = color;
	//gl_FragColor = vec4(diffuse,0.0,0.0,1.0);		
	gl_FragColor = finalColor;
}
