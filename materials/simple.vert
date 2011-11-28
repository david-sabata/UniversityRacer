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
uniform mat4 mv_inverse_transpose;

uniform vec3 eye;
uniform vec3 sight;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

uniform Material material;


out	vec4 ambientF;
out	vec4 diffuseF;
out	vec4 specularF;
flat out int shininessF;

/////////////////////////////////////////////////

varying vec3 normF;
varying vec3 halfVector;
varying vec3 lightDir;


/*
varying vec3 normalVec;
varying vec3 lightDir;
varying vec3 eyeVec;

out vec4 lightAmbient;
out vec4 lightDiffuse;
*/

void main() {

	// predpokladame enabledLights > 0
	vec4 lightPosition = lights[0]; // 0 == prvni hodnota prvniho svetla == pozice
	vec4 lightDiffuse = lights[1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	vec4 lightAmbient = lights[2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	
	//transformuje normaly do eyespace
	normF = vec3((view * model) * vec4(normal,0.0));
	
	//normalizujeme svetlo - u directional je pozice s vetla, rovnou smer
	lightDir = normalize(lightPosition.xyz);	

	halfVector = normalize(lightDir + eye); // TODO: je tohle spravny vypocet halfvectoru? podle wiki by mel
  
	//difuzni barva
	diffuseF = material.diffuse * lightDiffuse;
  
	//ambientni barva
	ambientF = material.ambient * lightAmbient;
  
	//prevod hodnot
	specularF = material.specular;
	shininessF = material.shininess;
	
		

	vec4 pos = vec4(position, 1);

	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;
	
	/*
	normalVec = vec3( mv_inverse_transpose * vec4(normal, 1) );

	vec3 vVertex = (mv * pos).xyz;
	lightDir = vec3(lightPosition.xyz - vVertex);
	eyeVec = -vVertex;
	*/
}
