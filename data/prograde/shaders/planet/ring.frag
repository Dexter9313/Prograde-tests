#version 150 core

in vec2 f_position;
in mat4 f_invrot;

uniform sampler2D tex;

uniform float inner;
uniform float outer;
uniform float planetradius;
uniform vec3 planetoblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 lightpos;
uniform float lightradius;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

out vec4 outColor;

// Disk on disk occlusion proportion (see notes/disk-occlusion.ggb in Geogebra
// geometry)
float getNeighborOcclusionFactor(float rLight, float rNeighbor, float dist)
{
	// both disks don't meet
	if(dist >= rLight + rNeighbor)
	{
		return 0.0;
	}

	// they meet and light is a point so occlusion == 1.0
	// early return because of divisions by sLight == 0.0
	if(rLight == 0.0)
	{
		return 1.0;
	}

	// surfaces of light disk and neighbor disk
	float sLight    = 3.1415 * rLight * rLight;
	float sNeighbor = 3.1415 * rNeighbor * rNeighbor;

	// disks intersection surface
	float sX = 0.0;

	// one disk is included in the other
	if(dist <= abs(rLight - rNeighbor))
	{
		sX = min(sLight, sNeighbor);
	}
	else
	{
		float alpha
		    = ((rLight * rLight) - (rNeighbor * rNeighbor) + (dist * dist))
		      / (2.0 * dist);
		float x = sqrt((rLight * rLight) - (alpha * alpha));

		float gammaLight = asin(x / rLight);
		if(alpha < 0.0)
		{
			gammaLight = 3.1415 - gammaLight;
		}
		float gammaNeighbor = asin(x / rNeighbor);
		if(alpha > dist)
		{
			gammaNeighbor = 3.1415 - gammaNeighbor;
		}

		sX = (rLight * rLight * gammaLight)
		     + (rNeighbor * rNeighbor * gammaNeighbor) - (dist * x);
	}

	return sX / sLight;
}

void main()
{
	// make perfect circles
	if(length(f_position) < inner || length(f_position) > outer)
		discard;

	float alt      = length(f_position);
	float texCoord = (alt - inner) / (outer - inner);

	vec3 pos           = vec3(f_position, 0.0);
	vec3 posRelToLight = pos - (f_invrot * vec4(lightpos, 1.0)).xyz;

	vec3 lightdir = -1.0 * normalize(posRelToLight);

	vec3 closestPoint = dot(lightdir, -1 * pos) * lightdir + pos;

	closestPoint /= planetoblateness;

	float coeff = 1.0;
	if(dot(lightdir, pos) < 0.0)
	{
		coeff -= getNeighborOcclusionFactor(lightradius * length(pos)
		                                        / length(posRelToLight),
		                                    planetradius, length(closestPoint));
	}

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = pos - (f_invrot * vec4(neighborsPosRadius[i].xyz, 1.0)).xyz;

		if(dot(lightdir, posRelToNeighbor) >= 0.0)
		{
			continue;
		}

		float neighborRadius = neighborsPosRadius[i].w;

		vec3 closestPoint = dot(lightdir, -1 * posRelToNeighbor) * lightdir
		                    + posRelToNeighbor;

		closestPoint /= neighborsOblateness[i];

		globalCoeffNeighbor *= (1.0
		                        - getNeighborOcclusionFactor(
		                              lightradius * length(posRelToNeighbor)
		                                  / length(posRelToLight),
		                              neighborRadius, length(closestPoint)));
	}
	// END NEIGHBORS

	outColor = texture(tex, vec2(texCoord, 0.5));
	outColor.rgb *= coeff * globalCoeffNeighbor;
}
