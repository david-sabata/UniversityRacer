#version 130

in vec2 t; 
out vec4 FragColor; 
varying vec4 diffuse;

void main() {
	vec4 texColor = vec4(1.0,0.0,0.0,1.0); 
	vec4 LightAmbient = vec4(0.2,0.2,0.0,0.2);
	FragColor = texColor * (LightAmbient + diffuse);	
}
