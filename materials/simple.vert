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
uniform mat3 mv_inverse_transpose;

uniform vec3 eye;
uniform vec3 sight;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

#define MAX_LIGHT 4
uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

uniform Material material;


//export vlastnosti materialu
out	vec4 ambientF[MAX_LIGHT];
out	vec4 diffuseF[MAX_LIGHT];
out	vec4 specularF;
flat out int shininessF;

/////////////////////////////////////////////////
out vec3 eyeLightPos[MAX_LIGHT];
out vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
out vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
out vec3 lightDir[MAX_LIGHT];


void main() {
	vec4 pos = vec4(position, 1.0);
	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;

	//transformace normaly do eyespace
	eyeNormal = normalize(mv_inverse_transpose * normal);

	//transformace zkoumaneho bodu do eyespace
	vec4 eyePosition4 = mv * pos;
	eyePosition = eyePosition4.xyz / eyePosition4.w;

	// predpokladame enabledLights > 0
	////////////////////////////SVETLO 1 /////////////////////////////////////

	vec4 lightPosition = lights[0 * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice
	vec4 lightDiffuse = lights[0 * 3 + 1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	vec4 lightAmbient = lights[0 * 3 + 2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace svetla do eyespace (TODO : zeptat se Davida kam ho umistil : pravdepodobne je ve worldspace?)
	vec4 lightPos4 = view * lightPosition;
	eyeLightPos[0] = lightPos4.xyz / lightPos4.w ;
	
	//smer paprsku dopadajiciho na plosku
	lightDir[0] = eyeLightPos[0] - eyePosition;

	//prevod hodnot
	diffuseF[0] = material.diffuse * lightDiffuse;
	ambientF[0] = material.ambient * lightAmbient;
	
	////////////////////////////SVETLO 2 /////////////////////////////////////

	lightPosition = lights[1 * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice
	lightDiffuse = lights[1 * 3 + 1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	lightAmbient = lights[1 * 3 + 2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace svetla do eyespace (TODO : zeptat se Davida kam ho umistil : pravdepodobne je ve worldspace?)
	lightPos4 = view * lightPosition;
	eyeLightPos[1] = lightPos4.xyz / lightPos4.w ;
	
	//smer paprsku dopadajiciho na plosku
	lightDir[1] = eyeLightPos[1] - eyePosition;

	//prevod hodnot
	diffuseF[1] = material.diffuse * lightDiffuse;
	ambientF[1] = material.ambient * lightAmbient;

	////////////////////////////SVETLO 3 /////////////////////////////////////

	lightPosition = lights[2 * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice
	lightDiffuse = lights[2 * 3 + 1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	lightAmbient = lights[2 * 3 + 2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace svetla do eyespace (TODO : zeptat se Davida kam ho umistil : pravdepodobne je ve worldspace?)
	lightPos4 = view * lightPosition;
	eyeLightPos[2] = lightPos4.xyz / lightPos4.w ;
	
	//smer paprsku dopadajiciho na plosku
	lightDir[2] = eyeLightPos[2] - eyePosition;
	
	//prevod hodnot
	diffuseF[2] = material.diffuse * lightDiffuse;
	ambientF[2] = material.ambient * lightAmbient;

	////////////////////////////SVETLO 4 /////////////////////////////////////

	lightPosition = lights[3 * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice
	lightDiffuse = lights[3 * 3 + 1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	lightAmbient = lights[3 * 3 + 2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace svetla do eyespace (TODO : zeptat se Davida kam ho umistil : pravdepodobne je ve worldspace?)
	lightPos4 = view * lightPosition;
	eyeLightPos[3] = lightPos4.xyz / lightPos4.w ;
	
	//smer paprsku dopadajiciho na plosku
	lightDir[3] = eyeLightPos[3] - eyePosition;

	//prevod hodnot
	diffuseF[3] = material.diffuse * lightDiffuse;
	ambientF[3] = material.ambient * lightAmbient;


	specularF = material.specular;
	shininessF = material.shininess;
		
}
