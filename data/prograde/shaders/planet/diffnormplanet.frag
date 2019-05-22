#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform samplerCube diff;
uniform samplerCube norm;

uniform float atmosphere;

uniform vec3 lightpos;
uniform vec4 neighborsPosRadius[5];

// only if custom mesh would it be -1, -1
uniform vec2 flipCoords = vec2(1.0, 1.0);

out vec4 outColor;

const float minR = 0.45;
const float maxR = 1.0;

const float minG = 0.45;
const float maxG = 1.0;

const float minB = 0.45;
const float maxB = 1.0;

void main()
{
	vec4 diffuse
	    = texture(diff, vec3(flipCoords * f_position.xy, f_position.z));
	vec3 normal
	    = texture(norm, vec3(flipCoords * f_position.xy, f_position.z)).xyz;
	normal = normalize(normal * 2.0 - 1.0); // from [0;1] to [-1;1]
	normal.xy *= flipCoords;

	// 0 or 1
	// avoids lighting stuff behind the planet
	// float coeff_pos = normalize(ceil(dot(normalize(lightpos),
	// normalize(f_position)) + 0.3));
	float coeff_pos = dot(normalize((f_invrot * vec4(lightpos, 1.0)).xyz),
	                      normalize(f_position));
	float coeff     = max(
        0.0, dot(normalize((f_invrot * vec4(lightpos, 1.0)).xyz), normal));

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	vec3 lightdir             = normalize((f_invrot * vec4(lightpos, 1.0)).xyz);
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = f_position
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

	outColor = diffuse;
	outColor.rgb *= coeff * globalCoeffNeighbor
	                * min(1.0, max(0.0, coeff_pos + 0.1) * 10.0);

	// Fully empirical eye-balled algorithm, FAR from accurate, no actual
	// scattering computed at all...
	// Fast as heck though
	float atmosColorGrad = coeff_pos * 0.5 + 0.5;
	float atmoscoeff     = clamp(atmosphere * (1.0 - coeff_pos), 0.0, 1.0);
	vec3 atmosCol        = vec3(0.0);
	if(atmosColorGrad > minR && atmosColorGrad < maxR)
	{
		atmosCol.r
		    = 0.5
		          * pow(-atmosColorGrad / (maxR - minR) + maxR / (maxR - minR),
		                15.0)
		          * 0.7
		      + 0.3;
	}
	if(atmosColorGrad > minG && atmosColorGrad < maxG)
	{
		atmosCol.g
		    = 0.3
		          * pow(-atmosColorGrad / (maxG - minG) + maxG / (maxG - minG),
		                1.0)
		          * 0.53
		      + 0.47;
	}
	if(atmosColorGrad > minB && atmosColorGrad < maxB)
	{
		atmosCol.b = 1.0 + atmosColorGrad / (maxB - minB)
		             - maxB / (maxB - minB) * 0.8 + 0.2;
	}

	atmosCol *= globalCoeffNeighbor;
	outColor.rgb = mix(outColor.rgb, atmosCol, atmoscoeff);
}
