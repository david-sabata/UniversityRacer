#version 130

in vec2 t; 
out vec4 FragColor; 

void main() { 
	vec4 tmp = vec4(t.x, t.y, 1, 1);
	FragColor = normalize(tmp);	
}