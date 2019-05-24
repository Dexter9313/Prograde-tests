#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform samplerCube diff;
uniform sampler2D texRing;

uniform float innerRing;
uniform float outerRing;

uniform vec3 oblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 lightpos;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

// only if custom mesh would it be -1, -1
uniform vec2 flipCoords = vec2(1.0, 1.0);

out vec4 outColor;

void main()
{
	vec3 pos      = normalize(f_position) * oblateness;
	vec3 norm     = normalize(normalize(f_position) / oblateness);
	vec3 lightdir = normalize((f_invrot * vec4(lightpos, 1.0)).xyz);

	// see if flip pos or f_position
	vec4 diffuse
	    = texture(diff, vec3(flipCoords * f_position.xy, f_position.z));

	float coeff = max(0.0, dot(lightdir, norm));

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = pos - (f_invrot * vec4(neighborsPosRadius[i].xyz, 1.0)).xyz;
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

	// RINGS SHADOW
	float coeffRings = 1.0;
	if(outerRing > 0.0 && sign(lightdir.z) != sign(pos.z))
	{
		vec3 pointOnRings
		    = pos + lightdir * abs(pos.z / lightdir.z);
		float alt = length(pointOnRings);
		if(alt >= innerRing && alt <= outerRing)
		{
			float texCoord = (alt - innerRing) / (outerRing - innerRing);
			coeffRings     = 1.0 - texture(texRing, vec2(texCoord, 0.5)).a;
		}
	}
	// END RINGS SHADOW

	outColor = diffuse;
	outColor.rgb *= coeffRings * coeff * globalCoeffNeighbor;
}
