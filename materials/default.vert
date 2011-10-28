#version 130

in vec3 position; 
in vec2 texpos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 t; 

void main() {
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1); 
	t = texpos;
}