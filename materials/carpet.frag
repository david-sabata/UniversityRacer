#version 130

// @LOAD materials/textures/carpet.bmp
uniform sampler2D tex;

// @LOAD materials/textures/carpetn.bmp
uniform sampler2D texNormal;

// @LOAD materials/textures/carpeth.bmp
uniform sampler2D texHeight;

varying vec3 lightVTan;
varying vec3 halfVector;

in vec2 texCoord;

in vec4 ambientF;
in vec4 diffuseF;
in vec4 specularF;
varying int shininessF;


void main() {
	
	float scale = 0.05f;
	float bias =  0.02f;
	
	vec3 normLightVTan = normalize(lightVTan);
	vec3 normHalfVector= normalize(halfVector);
	
	float height = texture(texHeight, texCoord).r * 2.0 - 1.0;
	height = scale * height + bias;
	vec2 texCoordNew = height * halfVector.xy + texCoord;
	
	
	vec3 offsetNormal = texture(texNormal, texCoordNew).xyz * 2.0 - 1.0;
	
	float normDotLight = max( dot( offsetNormal, normLightVTan ), 0.0);
	float normDotHalf = max(dot(offsetNormal, halfVector), 0.0);
	
	
	float power;
	if(normDotLight == 0.0)
		power = 0.0;
	else
		power = pow(normDotHalf, shininessF);
	
	vec4 finalColor = ambientF + diffuseF * normDotLight +
										specularF * power;
		
	vec4 texel = texture(tex, texCoordNew);
	
	gl_FragColor = texel * finalColor;
}
