#version 130

in vec2 t; 
out vec4 FragColor;
varying vec4 diffuse;
varying vec4 specular;

// @LOAD materials/textures/zed.bmp
uniform sampler2D texCarpet;

// @LOAD materials/textures/zed_normal.bmp
uniform sampler2D texCarpetNormal;

void main() {	
	vec4 LightAmbient = vec4(0.4, 0.4, 0.4, 0.4);
	vec4 texColor = texture(texCarpet, t);
	vec4 texColorNormal = texture(texCarpetNormal, t);
	
	vec3 bumpNormal = normalize(texture(texCarpetNormal, t).rgb * 2.0 - 1.0);
	float bumpDiffuse = max(dot(bumpNormal, normalize(gl_LightSource[0].position.xyz)), 0.0);
	
	FragColor =   ( specular +  texColor * (bumpDiffuse + LightAmbient + diffuse));
	//FragColor =  texColorNormal;
}
