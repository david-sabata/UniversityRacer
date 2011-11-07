#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
in vec3 normal;
in vec2 texpos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 eye;
uniform vec3 sight;

#define MAX_LIGHTS 8
uniform int enabledLights[MAX_LIGHTS]; // flagy pro jednotliva svetla [0/1]
/////////////////////////////////////////////////

varying vec4 diffuse; //vysledne svetlo
varying vec4 specular;
out vec2 t; 

void main() {
	
	//barva svetla nastavime na bilou
	vec4 LightDiffuse = vec4(1.0,1.0,1.0,1.0);
	vec4 LightSpecular = vec4(1.0,1.0,1.0,1.0);
	float MaterialShinines = 64;
	
	
  vec3 vertexNormal = gl_NormalMatrix * normal;	
  vec3 lightVector = normalize(gl_LightSource[0].position.xyz - position);
	
	float distance = length(lightVector);
	
	float attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
														 gl_LightSource[0].linearAttenuation * distance +
														 gl_LightSource[0].quadraticAttenuation * distance * distance);
														 
														 
	float reflex = max(dot(vertexNormal, lightVector), 0.0);//skalarni soucin vektoru svetla a normály polygonu,na ktery dopada svetlo
	//~ diffuse = LightDiffuse * diffuse;
	diffuse = LightDiffuse * reflex * attenuation;
	
	float specPow = 0.0;
	if(reflex != 0) {
		vec3 cameraVector = normalize(eye - position);
		vec3 halfVector = normalize(lightVector + cameraVector);
		float specReflex = max(dot(vertexNormal, halfVector), 0.0);
		specPow = pow(specReflex, MaterialShinines);
	}
	specular = LightSpecular * specPow * attenuation;
	
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
	
	t = texpos;
}
