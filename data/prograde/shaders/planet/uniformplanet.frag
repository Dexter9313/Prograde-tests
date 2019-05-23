#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform vec3 color;
uniform vec3 lightpos;
uniform vec4 neighborsPosRadius[5];

out vec4 outColor;

void main()
{
	float coeff = max(0.0, dot(normalize((f_invrot * vec4(lightpos, 1.0)).xyz),
	                           normalize(f_position)));

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	vec3 lightdir             = normalize((f_invrot * vec4(lightpos, 1.0)).xyz);
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = normalize(f_position)
		      - (f_invrot * vec4(neighborsPosRadius[i].xyz, 1.0)).xyz;
		float neighborRadius = neighborsPosRadius[i].w;

		vec3 closestPoint = dot(lightdir, -1 * posRelToNeighbor) * lightdir
		                    + posRelToNeighbor;

		float coeffNeighbor = 1.0;
		if(length(closestPoint) < neighborRadius
		   && dot(lightdir, posRelToNeighbor) < 0.0)
			coeffNeighbor = pow(length(closestPoint) / neighborRadius, 500);

		globalCoeffNeighbor *= coeffNeighbor;
	}
	// END NEIGHBORS

	outColor = vec4(color, 1.0);
	outColor.rgb *= coeff * globalCoeffNeighbor;
}
