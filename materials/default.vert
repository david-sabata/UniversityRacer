#version 130

in vec3 position; 
in vec2 texpos;

uniform mat4 mvp; 

out vec2 t; 

void main() { 
	gl_Position = mvp * vec4(position,1); 
	t = texpos;
}