#version 130
#define MAX_LIGHTS 4

in vec3 position;  // pozice vertexu
in vec3 normal; // normaly
in vec2 texpos; //texturovaci souradnice

//matice
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat3 mv_inverse_transpose;

//vlastnosti materialu
struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};
uniform Material material;

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

out vec3 eyeLightPos[MAX_LIGHTS]; //pozice svetel v prostoru OKA
out vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
out vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA

out vec2 t; //texturovaci souradnice se rpadavaji do fragment shaderu

void main() {
	vec4 pos = vec4(position, 1.0);
	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;
	t = texpos;

	//transformace normaly do eyespace
	eyeNormal = normalize(mv_inverse_transpose * normal);

	//transformace zkoumaneho bodu do eyespace
	vec4 eyePosition4 = mv * pos;
	eyePosition = eyePosition4.xyz / eyePosition4.w;

	////////////////////////////SVETLO /////////////////////////////////////
	// predpokladame enabledLights > 0
	for(int i =0; i < enabledLights; i++) {
		vec4 lightPosition = lights[i * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice

		//transformace svetla z worldspace  do eyespace
		vec4 lightPos4 = view * lightPosition;
		eyeLightPos[i] = lightPos4.xyz / lightPos4.w ;
	} 
}
