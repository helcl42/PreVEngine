// cone step mapping client code

vec2 ConeStepMapping(in sampler2D coneMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec2 ddx, in vec2 ddy, in vec3 texDir3D)
{
    vec2 R = normalize(vec2(length(texDir3D.xy), texDir3D.z));
    vec2 P = R * heightScale / texDir3D.z;

    vec2 textureSize = textureSize(coneMapSampler, 0);
    vec2 minTextureStep = normalize(texDir3D.xy) / textureSize;
    float minStep = length(minTextureStep) * 1.0 / R.x;

    float t = 0.0;
    for (uint i = 0; i < numLayers; ++i)
    {
        vec3 samplePoint = vec3(uv.xy, heightScale) + texDir3D * t;

        vec2 heightAndCone = clamp(textureGrad(coneMapSampler, samplePoint.xy, ddx, ddy).rg, 0.0, 1.0);
        float h = heightAndCone.x * heightScale;
        float c = heightAndCone.y * heightAndCone.y / heightScale; // cone ratio is stored as sqrt(cone_ratio) due to better distribution so we need to multiply it by itself to get real cone_ratio

        vec2 C = P + R * t;
        if (C.y <= h)
		{
        	break;
		}

        vec2 Q = vec2(C.x, h);
        vec2 S = normalize(vec2(c, 1.0));
        float newT = dot(Q - P, vec2(S.y, -S.x)) / dot(R, vec2(S.y, -S.x));
        t = max(t + minStep, newT);
    }

    vec2 texC = uv.xy + texDir3D.xy * t;
    return texC.xy;
}

// relaxed cone step mapping client code

const bool useDepthBias = true;

vec3 GetRayDirection(in vec3 viewDirection, in float heightScale)
{
	vec3 v = normalize(viewDirection);
	v.z = abs(v.z);
	if (useDepthBias)
	{
		float db = 1.0 - v.z;
		db *= db;
		db *= db;
		db = 1.0 - db * db;
		v.xy *= db;
	}
	v.xy *= heightScale;
    return v;
}

float GetInverseHeight(float height)
{
    return 1.0 - height;
}

vec2 RelaxedConeStepMapping(in sampler2D coneMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec2 ddx, in vec2 ddy, in vec3 texDir3D)
{
    const uint binarySteps = 6;

    vec3 rayPos = vec3(uv, 0.0);
    vec3 rayDir = GetRayDirection(texDir3D, heightScale);

    rayDir /= rayDir.z; // Scale rayDir
    float rayRatio = length(rayDir.xy);
    vec3 pos = rayPos;
    for(uint i = 0; i < numLayers; ++i)
    {
        vec2 heightAndCone = clamp(textureGrad(coneMapSampler, pos.xy, ddx, ddy).rg, 0.0, 1.0);
        float coneRatio = heightAndCone.g * heightAndCone.g; // cone ratio is stored as sqrt(cone_ratio) due to better distribution so we need to multiply it by itself to get real cone_ratio
        float height = GetInverseHeight(heightAndCone.r) - pos.z;
        float d = coneRatio * height / (rayRatio + coneRatio);
        pos += rayDir * d;
    }

    // Binary search initial range and initial position
    vec3 bsRange = 0.5 * rayDir * pos.z;
    vec3 bsPosition = rayPos + bsRange;
    for(uint i = 0; i < binarySteps; ++i)
    {
        vec2 heightAndCone = clamp(textureGrad(coneMapSampler, pos.xy, ddx, ddy).rg, 0.0, 1.0);
        bsRange *= 0.5;
        if (bsPosition.z < GetInverseHeight(heightAndCone.r)) // If outside
        {
            bsPosition += bsRange; // Move forward
        }
        else
        {
            bsPosition -= bsRange; // Move backward
        }
    }
    return bsPosition.xy;
}
