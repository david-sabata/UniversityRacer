#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
in vec2 texpos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 eye;
uniform vec3 sight;

#define MAX_LIGHTS 8
uniform int enabledLights[MAX_LIGHTS]; // flagy pro jednotliva svetla [0/1]
/////////////////////////////////////////////////

varying vec4 diffuse; //rozptyl svetla
out vec2 t; 

void main() {
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
	
	//vec3 normal = gl_Normal;
	vec3 normal = vec3(1.0,0.2,0.2);
	
	vec3 lightVector = normalize(gl_LightSource[0].position.xyz);
	float reflex = max(0.0, dot(normal, lightVector));//skalarni soucin vektoru svetla a normály polygonu,na ktery dopada svetlo
	diffuse = gl_LightSource[0].diffuse * reflex;
	t = texpos;
}
