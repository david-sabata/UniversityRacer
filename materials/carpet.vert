#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
in vec3 normal;
in vec2 texpos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 eye;
uniform vec3 sight;

#define MAX_LIGHTS 8
uniform int enabledLights[MAX_LIGHTS]; // flagy pro jednotliva svetla [0/1]

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform Material material;
/////////////////////////////////////////////////

varying vec3 lightVTan; //svetlo v tangent space
varying vec3 halfVector;

out vec2 texCoord;

out	vec4 ambientF;
out	vec4 diffuseF;
out	vec4 specularF;
varying	int shininessF;


//provizorni reseni!!!!!!Nutno spolecne s normalami dodavat tangenty - jak to udelat je zde:
//http://fabiensanglard.net/bumpMapping/index.php


void main() {
   //vlastnosti svetla
  vec4 lightDiffuse = vec4(1.0,1.0,1.0,1.0);
	vec4 lightAmbient = vec4(0.2,0.2,0.2,1.0);
	
	//spocitani halfvectoru
	vec3 eyePosition = eye;
	vec3 V = normalize(-eyePosition);
	vec3 L = normalize(gl_LightSource[0].position.xyz - eyePosition);
	halfVector = normalize(L + V);
	
  //////////////////PARALLAX///////////////////////////	
	vec4 vertexDiffuse, ambient, globalAmbient;
	float NdotL;
	
	//spocitame nakloneni plosky pro tangent space
	vec3 normNormal = normalize(vec3((view * model) * vec4(normal,0.0)));
	vec3 tangent = vec3(1.0,0.0,0.0);
	vec3 binormal = cross(tangent, normNormal);
	
	//vytvorime transformacni matici pro tangent space
	mat3 tanMatrix = mat3(tangent.x, binormal.x, normNormal.x,
												tangent.y, binormal.y, normNormal.y,
												tangent.z, binormal.z, normNormal.z
	);
	
	//vypocitame vektor svetla pro dany vertex od zdroje
	vec3 lightVector = gl_LightSource[0].position.xyz;
	//prevedeme vektor svetla do tangent space
	lightVTan = lightVector * tanMatrix;
	
	
	halfVector = halfVector * tanMatrix;
	
	
		//difuzni barva
  diffuseF = material.diffuse * lightDiffuse;
  
  //ambientni barva
  ambientF = material.ambient * lightAmbient;
  
  //prevod hodnot
  specularF = material.specular;
  shininessF = material.shininess;
  
  
	//predavani promennych do fragment schaderu
	texCoord = texpos;
	
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
}
