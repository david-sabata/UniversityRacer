#version 130




varying vec3 normF, halfVector;
varying vec3 vertexF;
varying vec3 lightDir;
varying vec3 eyeV;
varying float distance;

in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
in int shininessF;

in vec2 t;

void main() {

	//lambert - diffuse
	vec3 lam = normalize(lightDir);
	vec3 nF = normalize(normF);
	
	//pod jakym uhlem dopada svetlo na plosku
	float normDotLam = max(dot(nF,lam),0.0);
	//upravime barvu plosky dle odrazivosti
	vec4 diff =  diffuseF * normDotLam;
	diff = clamp(diff, 0.0, 1.0);
	
	//ambient - okolni svetlo
	vec4 amb =  ambientF;
	
	//specualar - reflection
	vec3 halfV = normalize(halfVector);
	float normDotHV = max(dot(nF,halfV),0.0);
	
	//TODO nejak sem dostat odrazivost materialu !!!!!!!
	vec4 spec = specularF * pow(normDotHV, 64); //shininess - 64
	spec = clamp(spec, 0.0 , 1.0);
	
	
	//vysledna barva
	gl_FragColor =  (amb + diff + spec);
}
