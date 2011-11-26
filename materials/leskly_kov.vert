#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
in vec3 normal;
in vec2 texpos;

out vec2 t;
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

out	vec4 ambientF;
out	vec4 diffuseF;
out	vec4 specularF;
out	int shininessF;

uniform Material material;
/////////////////////////////////////////////////

varying vec3 normF, halfVector;
varying vec3 vertexF;
varying vec3 lightDir;
varying vec3 eyeV;
varying float distance;

void main() {
	
	//transformuje normaly do eyespace
	normF = vec3((view * model) * vec4(normal,0.0));
	
	//normalizujeme svetlo - u directional je pozice s vetla, rovnou smer
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz);	

  halfVector = normalize(gl_LightSource[0].halfVector.xyz);
  
  //vlastnosti svetla
  vec4 lightDiffuse = vec4(1.0,1.0,1.0,1.0);
	vec4 lightAmbient = vec4(0.2,0.2,0.2,1.0);
	
	//difuzni barva
  diffuseF = material.diffuse * lightDiffuse;
  
  //ambientni barva
  ambientF = material.ambient * lightAmbient;
  
  //prevod hodnot
  specularF = material.specular;
  shininessF = material.shininess;

  
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
	
	t = texpos;
}
