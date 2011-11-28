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

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

out	vec4 ambientF;
out	vec4 diffuseF;
out	vec4 specularF;
flat out int shininessF;

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

uniform Material material;
/////////////////////////////////////////////////

varying vec3 normF, halfVector;
varying vec3 vertexF;
varying vec3 lightDir;
varying vec3 eyeV;
varying float distance;

void main() {
	
	// predpokladame enabledLights > 0
	vec4 lightPosition = lights[0]; // 0 == prvni hodnota prvniho svetla == pozice
	vec4 lightDiffuse = lights[1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	vec4 lightAmbient = lights[2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformuje normaly do eyespace
	normF = vec3((view * model) * vec4(normal,0.0));
	
	//normalizujeme svetlo - u directional je pozice s vetla, rovnou smer
	vec3 lightDir = normalize(lightPosition.xyz);	

	halfVector = normalize(normalize(lightPosition.xyz) + normalize(eye)); // TODO: je tohle spravny vypocet halfvectoru? podle wiki by mel
  
	//difuzni barva
	diffuseF = material.diffuse * lightDiffuse;
  
	//ambientni barva
	ambientF = material.ambient * lightAmbient;
  
	//prevod hodnot
	specularF = material.specular;
	shininessF = material.shininess;

  
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position,1);
}
