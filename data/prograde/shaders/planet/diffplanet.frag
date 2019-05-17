#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform samplerCube diff;

uniform vec3 lightpos;

// only if custom mesh would it be -1, -1
uniform vec2 flipCoords = vec2(1.0, 1.0);


out vec4 outColor;

void main()
{
	vec4 diffuse = texture(diff, vec3(flipCoords * f_position.xy, f_position.z));

	float coeff = max(0.0, dot(normalize((f_invrot * vec4(lightpos, 1.0)).xyz),
	                           normalize(f_position)));

	outColor = diffuse;
	outColor.rgb *= coeff;
}
