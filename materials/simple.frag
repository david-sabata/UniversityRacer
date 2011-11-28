#version 130


varying vec3 normF;
varying vec3 halfVector;
varying vec3 lightDir;


in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
flat in int shininessF;

/*
in vec4 lightAmbient;
in vec4 lightDiffuse;

varying vec3 normalVec;
varying vec3 lightDir;
varying vec3 eyeVec;
*/

void main() {
	
	//lambert - diffuse
	vec3 lam = normalize(lightDir);
	vec3 nF = normalize(normF);
	
	//pod jakym uhlem dopada svetlo na plosku
	float normDotLam = max(dot(nF, lam), 0.0);
	
	//upravime barvu plosky dle odrazivosti
	vec4 diff =  diffuseF * normDotLam;
	diff = clamp(diff, 0.0, 1.0);
	
	//ambient - okolni svetlo
	vec4 amb =  ambientF;
	
	//specular - reflection
	vec3 halfV = normalize(halfVector);
	float normDotHV = max(dot(nF, halfV), 0.0);
		
	vec4 spec = specularF * pow(normDotHV, shininessF);
	spec = clamp(spec, 0.0 , 1.0);
	
	//vysledna barva
	gl_FragColor =  amb + diff + spec;
	

	/*
	vec4 final_color = lightAmbient * ambientF;
	
	vec3 N = normalize(normalVec);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += lightDiffuse * diffuseF * lambertTerm;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), shininessF );
		final_color += lightAmbient * specularF * specular;	
	}	

	gl_FragColor = final_color;	
	*/
}
