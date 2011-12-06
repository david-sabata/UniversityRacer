#version 130

////////////////////////////////////////////////////////////////////////////////
// Kresleni probiha mimo Scene, pouzijeme jen vstupy ktere se nam hodi,
// zbytek si dohledame sami (namisto ShaderManageru) volanim getUniformLocation.
// Kreslit se budou ctverce, v texture bude jediny pevne dany font.
////////////////////////////////////////////////////////////////////////////////
in vec2 position;
in vec2 texpos;

//uniform mat4 mvp;
uniform mat4 projection;

out vec2 texcoords;
/////////////////////////////////////////////////


void main() {	
	texcoords = texpos;
	gl_Position = vec4(position, 0, 1);
}
