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

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

uniform Material material;


//export vlastnosti materialu
out	vec4 ambientF;
out	vec4 diffuseF;
out	vec4 specularF;
flat out int shininessF;

/////////////////////////////////////////////////
out vec3 eyeLightPos;
out vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
out vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
out vec3 lightDir;

out vec4 color;

void main() {
	vec4 pos = vec4(position, 1.0);
	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;


	// predpokladame enabledLights > 0
	vec4 lightPosition = lights[0]; // 0 == prvni hodnota prvniho svetla == pozice
	vec4 lightDiffuse = lights[1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	vec4 lightAmbient = lights[2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace normaly do eyespace
	eyeNormal = normalize(mv_inverse_transpose * normal);
	
	//transformace zkoumaneho bodu do eyespace
	vec4 eyePosition4 = mv * pos;
	eyePosition = eyePosition4.xyz / eyePosition4.w;

	//transformace svetla do eyespace (TODO : zeptat se Davida kam ho umistil : pravdepodobne je ve worldspace?)
	vec4 lightPos4 = view * lightPosition;
	eyeLightPos = lightPos4.xyz / lightPos4.w ;
	
	//smer paprsku dopadajiciho na plosku
	lightDir = eyeLightPos - eyePosition;

	//prevod hodnot
	diffuseF = material.diffuse * lightDiffuse;
	ambientF = material.ambient * lightAmbient;
	specularF = material.specular;
	shininessF = material.shininess;
		
}
