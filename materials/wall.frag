#version 130

// @LOAD materials/textures/rocks.bmp
uniform sampler2D tex;

// @LOAD materials/textures/rocksnm.bmp
uniform sampler2D texNormal;

// @LOAD materials/textures/rockshm.bmp
uniform sampler2D texHeight;

varying vec3 lightVTan;
varying vec3 viewDirTan;

varying vec4 diffuse;
in vec2 texCoord;

void main() {
	
	float scale = 0.08f;
	float bias =  0.02f;
	
	vec3 normLightVTan = normalize(lightVTan);
	vec3 normViewDirTan = normalize(viewDirTan);
	
	float height = texture(texHeight, texCoord).r * 2.0 - 1.0;
	height = scale * height - bias;
	vec2 texCoordNew = height * normViewDirTan.xy + texCoord;
	
	
	vec3 offsetNormal = texture(texNormal, texCoordNew).xyz * 2.0 - 1.0;
	
	float diffContrib = max( dot( offsetNormal, normLightVTan ), 0.0);
	vec4 texel = texture(tex, texCoordNew);
	
	gl_FragColor = texel * diffContrib;
}
