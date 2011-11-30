#version 130


varying vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
varying vec3 eyeLightVector; // vektor ze svetla do zkoumaneho bodu v prostoru OKA
varying vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA

//potreba pro spekularni odlesky
varying vec3 halfVector;


//import vlastnosti materialu
in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
flat in int shininessF;

in vec4 color;

void main() {
	
	vec4 finalColor = ambientF; //ambientF obsahuje i ambientni slozku svetla
	
	vec3 N = normalize(eyeNormal);
	vec3 L = normalize(eyeLightVector);
	
	float diffuse = max(dot(N, L), 0.0);
	finalColor += diffuse * diffuseF;
	
	
	//~ float attenuation, distance;
		//~ 
	//~ //pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
	//~ float constantAtt = 1.0;
	//~ float linearAtt = 0.00012;
	//~ float quadraticAtt = 0.0000006;
	//~ attenuation = 1.0 / (constantAtt + linearAtt * distance +
											 //~ quadraticAtt * distance * distance);
				
	gl_FragColor = finalColor;
}
