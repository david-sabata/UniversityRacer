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

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform Material material;
/////////////////////////////////////////////////


void main() {
	vec4 lightDiffuse = vec4(0.8, 0.8, 0.8, 0.8);
	vec4 lightAmbient = vec4(0.5, 0.5, 0.5, 0.5);

	vec3 vertexNormal, lightDir;
	vec4 vertexDiffuse, ambient, globalAmbient;
	float NdotL;

	vertexNormal = normalize(gl_NormalMatrix * normal);
	lightDir = normalize(vec3(gl_LightSource[0].position));
	NdotL = max(dot(vertexNormal, lightDir), 0.0);
	//vertexDiffuse = material.diffuse * gl_LightSource[0].diffuse;
	vertexDiffuse = material.diffuse * lightDiffuse;
	/* Compute the ambient and globalAmbient terms */

	ambient = material.ambient * lightAmbient;
	globalAmbient = vec4(0.2, 0.2, 0.2, 1.0) * material.ambient;
	gl_FrontColor =  NdotL * vertexDiffuse + globalAmbient + ambient;
	//gl_FrontColor = material.ambient;
	//gl_FrontColor = vec4(0.3, 0.3, 0.3, 1.0);

	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
}
