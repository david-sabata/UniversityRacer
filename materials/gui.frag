#version 130



in vec2 texcoords;

// barva pozadi
uniform vec4 background; 

// @LOAD font.bmp
uniform sampler2D tex;



void main() {	
	vec4 texcolor = texture(tex, texcoords);

	if (texcolor != vec4(1.0f, 0.0f, 1.0f, 1.0f)) // klicovaci barva #ff00ff
		gl_FragColor = texcolor;
	else
		gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}
