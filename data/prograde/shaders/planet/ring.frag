#version 150 core

in vec2 f_position;
in mat4 f_invrot;

uniform sampler2D tex;

uniform float inner;
uniform float outer;
uniform float planetradius;
uniform vec3 planetoblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 lightpos;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

out vec4 outColor;

void main()
{
	// make perfect circles
	if(length(f_position) < inner || length(f_position) > outer)
		discard;

	float alt      = length(f_position);
	float texCoord = (alt - inner) / (outer - inner);

	vec3 lightdir = normalize((f_invrot * vec4(lightpos, 1.0)).xyz);

	vec3 closestPoint = dot(lightdir, -1 * vec3(f_position, 0.0)) * lightdir
	                    + vec3(f_position, 0.0);

	closestPoint /= planetoblateness;

	float coeff = 1.0;
	if(length(closestPoint) < planetradius
	   && dot(lightdir, vec3(f_position, 0.0)) < 0.0)
		coeff = pow(length(closestPoint) / planetradius, 500);

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = vec3(f_position, 0.0)
		      - (f_invrot * vec4(neighborsPosRadius[i].xyz, 1.0)).xyz;
		float neighborRadius = neighborsPosRadius[i].w;

		vec3 closestPoint = dot(lightdir, -1 * posRelToNeighbor) * lightdir
		                    + posRelToNeighbor;

		closestPoint /= neighborsOblateness[i];

		float coeffNeighbor = 1.0;
		if(length(closestPoint) < neighborRadius
		   && dot(lightdir, posRelToNeighbor) < 0.0)
			coeffNeighbor = pow(length(closestPoint) / neighborRadius, 500);

		globalCoeffNeighbor *= coeffNeighbor;
	}
	// END NEIGHBORS

	outColor = texture(tex, vec2(texCoord, 0.5));
	outColor.rgb *= coeff * globalCoeffNeighbor;
}
