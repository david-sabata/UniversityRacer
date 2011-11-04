#version 130

in vec2 t; 
out vec4 FragColor;
varying vec4 diffuse;

// @LOAD materials/tiles.bmp
uniform sampler2D textura;

void main() {		
	vec4 LightAmbient = vec4(0.2,0.2,0.0,0.2);
	vec4 texColor = texture2D(textura, t);
	FragColor = texColor * (LightAmbient + diffuse);	
}
