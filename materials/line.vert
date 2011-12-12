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


void main() {
	vec4 pos = vec4(position, 1);
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;	
}
