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

varying vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
varying vec3 eyeLightVector; // vektor ze svetla do zkoumaneho bodu v prostoru OKA
varying vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
out vec4 color;

void main() {

	// predpokladame enabledLights > 0
	vec4 lightPosition = lights[0]; // 0 == prvni hodnota prvniho svetla == pozice
	vec4 lightDiffuse = lights[1]; // 1 == druha hodnota prvniho svetla == difuzni slozka
	vec4 lightAmbient = lights[2]; // 2 == treti hodnota prvniho svetla == ambientni slozka

	//transformace normaly do prostoru OKA
	eyeNormal = normalize(mv_inverse_transpose *  normal);
	
	//transformace zkoumaneho bodu do prostoru OKA
	eyePosition = vec3(( view *  model) * vec4(position,1.0));

	//vektor svetla mezi okem a zkoumanym bodem
	eyeLightVector = vec3(lightPosition.xyz - eyePosition.xyz);	
   
	

	//prevod hodnot
	diffuseF = material.diffuse * lightDiffuse;
	ambientF = material.ambient * lightAmbient;
	specularF = material.specular;
	shininessF = material.shininess;
	
	vec4 pos = vec4(position, 1);

	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;
	
}
