#version 130

in vec2 t; 
out vec4 FragColor;
varying vec4 diffuse;

// @LOAD materials/dummy2.bmp
uniform sampler2D blabla;

// @LOAD materials/dummy.bmp
uniform sampler2D texCarpet;

void main() {		
	vec4 LightAmbient = vec4(0.2, 0.2, 0.2, 0.2);
	vec4 texColor = mix(texture(texCarpet, t), texture(blabla, t), 0.5);
	FragColor = texColor * (LightAmbient + diffuse);
}
