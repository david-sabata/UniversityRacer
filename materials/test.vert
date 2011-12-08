#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
//in vec3 normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

/////////////////////////////////////////////////


void main() {
	vec4 pos = vec4(position, 1.0);
	//mat4 mv = view * model;
	//mat4 mvp = projection * view * model;
	mat4 mvp = projection * view;
	gl_Position = mvp * pos;	
}
