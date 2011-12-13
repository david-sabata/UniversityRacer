// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tomáš Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tomáš Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David Šabata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#version 130



in vec2 texcoords;

// barva pozadi
uniform vec4 background; 

uniform sampler2D texture1;



void main() {	
	vec4 texcolor = texture(texture1, texcoords);

	if (texcolor != vec4(1.0f, 0.0f, 1.0f, 1.0f)) // klicovaci barva #ff00ff
		gl_FragColor = texcolor;
	else
		gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}
