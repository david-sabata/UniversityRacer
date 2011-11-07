#version 130

in vec2 t; 
out vec4 FragColor;
varying vec4 diffuse;
varying vec4 specular;

// @LOAD materials/dummy.bmp
uniform sampler2D textura;

void main() {		
	vec4 LightAmbient = vec4(0.2, 0.2, 0.2, 0.2);
	vec4 texColor = texture2D(textura, t);
	FragColor = specular + texColor * (LightAmbient + diffuse);

}
