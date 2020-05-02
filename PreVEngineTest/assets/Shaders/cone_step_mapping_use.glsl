// cone step mapping client code

const float minDotToApplyConeMapping = 0.001;

vec2 ConeStepMapping(in sampler2D coneMapSampler, in float heightScale, in uint numLayers, in vec2 uv, in vec3 texDir3D)
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

        vec2 heightAndCone = clamp(texture(coneMapSampler, samplePoint.xy).rg, 0.0, 1.0);
        float h = heightAndCone.x * heightScale;
        float c = heightAndCone.y * heightAndCone.y / heightScale;

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
    
    vec2  texC = uv.xy + texDir3D.xy * t;
    return texC.xy;
}
