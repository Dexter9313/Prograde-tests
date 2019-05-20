#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform vec3 color;
uniform vec3 lightpos;


out vec4 outColor;

void main()
{
	float coeff = max(0.0, dot(normalize((f_invrot * vec4(lightpos, 1.0)).xyz),
	                           normalize(f_position)));

	outColor = vec4(color, 1.0);
	outColor.rgb *= coeff;
}
