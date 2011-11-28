#version 130


varying vec3 normF;
varying vec3 halfVector;
varying vec3 lightVec;
varying float distance;

in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
flat in int shininessF;

void main() {
	
	//lambert - diffuse
	vec3 lam = normalize(lightVec);
	vec3 nF = normalize(normF);

	//pod jakym uhlem dopada svetlo na plosku
	float normDotLam = max(dot(nF, lam), 0.0);
	
	//ztrata intenzity svetla zavisla na vzdalenosti
	float attenuation;
	
	//pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
	float constantAtt = 1.0;
	float linearAtt = 0.0014;
	float quadraticAtt = 0.000007;
	attenuation = 1.0 / (constantAtt + linearAtt * distance +
										 quadraticAtt * distance * distance);
		  
	//upravime barvu plosky dle odrazivosti
	vec4 diff =  diffuseF * normDotLam;

			
	//ambient - okolni svetlo
	vec4 amb =  ambientF;
	
	//specular - reflection
	vec3 HV = normalize(halfVector);
	float normDotHV = max(dot(nF, HV), 0.0);
		
	vec4 spec = specularF * pow(normDotHV, 64);
			
	
	//vysledna barva
	gl_FragColor =  attenuation * (amb + diff + spec);
	//~ gl_FragColor =  vec4(attenuation,attenuation,attenuation,1.0);
	
	

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
