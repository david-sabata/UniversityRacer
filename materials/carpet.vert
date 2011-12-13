// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#version 130

#define MAX_LIGHTS 4

in vec3 position; 
in vec3 normal;
in vec2 texpos;
in vec3 tangent;

uniform mat4 view;
uniform mat4 model;
uniform mat4 modelView;
uniform mat4 modelViewProjection;
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

//texturovaci souradnice predavane do fragment sahderu
out vec2 t;

//tangent space
out vec3 tanCam; // pozice kamery v tangent space
out vec3 tanLightDir[MAX_LIGHTS]; //smer svetla v tangent space

void main() {
	vec3 eyeLightPos[MAX_LIGHTS]; // pozice svetel v prostoru OKA
	vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
	vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA
	t = texpos; // prirazeni texturovacich souradnic do fragment shaderu

	vec4 pos = vec4(position, 1.0);
	gl_Position = modelViewProjection * pos;

	//transformace normaly do prostoru OKA
	eyeNormal = normalize(mv_inverse_transpose * normal);
	
	//transformace zkoumaneho bodu do prostoru OKA
	vec4 eyePosition4 = modelView * pos;
	eyePosition = eyePosition4.xyz / eyePosition4.w;
	
	// Transformujeme pozice svetel do eyespace
	// predpokladame enabledLights > 0				
	for(int i=0; i < enabledLights; i++) {
		vec4 lightPosition = lights[i * 3 + 0]; // i - svetlo
		//transformace svetla do eyespace
		vec4 lightPos4 = view * lightPosition;
		eyeLightPos[i] = lightPos4.xyz / lightPos4.w ;
	}


	///////////////////////////TANGENT SPACE MATICE///////////////////////////////
	//priprava pro PARALLAX MAPPING
    vec3 eyeTan = normalize(mv_inverse_transpose * tangent);
	
	//vektorovym soucinem ziskame binormaly
	vec3 binormal = normalize(cross(eyeNormal, eyeTan ));

	//vytvorime matici tbnMatrix, ktera bude transformovat body do TangentSpace
	mat3 tbnMatrix = mat3(eyeTan, binormal, eyeNormal);

	/////////////////////////TRANSFORM TO TANGENT SPACE/////////////////////
	vec3 eyeCam = -eyePosition;
	//prevedeme kameru, pozici svetla, plosku do tangent space
	tanCam = eyeCam * tbnMatrix;

	//prevod svetel do tangent space
	for( int i = 0; i < enabledLights; i++) {
		vec3 lightDir = eyeLightPos[i] - eyePosition;
		tanLightDir[i] = lightDir * tbnMatrix;
	}	
}
