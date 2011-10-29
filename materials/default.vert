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


out vec2 t; 

void main() {
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1); 
	t = texpos;
}