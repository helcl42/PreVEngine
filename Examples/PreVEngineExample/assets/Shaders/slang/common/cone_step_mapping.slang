// Cone step mapping module
module cone_step_mapping;

static const bool useDepthBias = true;

float3 GetRayDirection(float3 viewDirection, float heightScale)
{
    float3 v = normalize(viewDirection);
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

public float2 ConeStepMapping(Texture2D coneMapTexture, SamplerState coneMapSampler, float heightScale, uint numLayers, float2 uv, float2 ddxUV, float2 ddyUV, float3 texDir3D)
{
    float2 R = normalize(float2(length(texDir3D.xy), texDir3D.z));
    float2 P = R * heightScale / texDir3D.z;

    uint width, height;
    coneMapTexture.GetDimensions(width, height);
    float2 textureSize = float2((float)width, (float)height);
    float2 minTextureStep = normalize(texDir3D.xy) / textureSize;
    float minStep = length(minTextureStep) * 1.0 / R.x;

    float t = 0.0;
    for (uint i = 0; i < numLayers; ++i)
    {
        float3 samplePoint = float3(uv.xy, heightScale) + texDir3D * t;

        float2 heightAndCone = clamp(coneMapTexture.SampleGrad(coneMapSampler, samplePoint.xy, ddxUV, ddyUV).rg, 0.0, 1.0);
        float h = heightAndCone.x * heightScale;
        float c = heightAndCone.y * heightAndCone.y / heightScale;

        float2 C = P + R * t;
        if (C.y <= h)
        {
            break;
        }

        float2 Q = float2(C.x, h);
        float2 S = normalize(float2(c, 1.0));
        float newT = dot(Q - P, float2(S.y, -S.x)) / dot(R, float2(S.y, -S.x));
        t = max(t + minStep, newT);
    }

    float2 texC = uv.xy + texDir3D.xy * t;
    return texC.xy;
}

// Texture2DArray overload: samples the cone map from array `layer`. Same algorithm as the Texture2D
// version above - only the sample source (and GetDimensions element count) differ.
public float2 ConeStepMapping(Texture2DArray coneMapTextures, SamplerState coneMapSampler, uint layer, float heightScale, uint numLayers, float2 uv, float2 ddxUV, float2 ddyUV, float3 texDir3D)
{
    float2 R = normalize(float2(length(texDir3D.xy), texDir3D.z));
    float2 P = R * heightScale / texDir3D.z;

    uint width, height, elements;
    coneMapTextures.GetDimensions(width, height, elements);
    float2 textureSize = float2((float)width, (float)height);
    float2 minTextureStep = normalize(texDir3D.xy) / textureSize;
    float minStep = length(minTextureStep) * 1.0 / R.x;

    float t = 0.0;
    for (uint i = 0; i < numLayers; ++i)
    {
        float3 samplePoint = float3(uv.xy, heightScale) + texDir3D * t;

        float2 heightAndCone = clamp(coneMapTextures.SampleGrad(coneMapSampler, float3(samplePoint.xy, (float)layer), ddxUV, ddyUV).rg, 0.0, 1.0);
        float h = heightAndCone.x * heightScale;
        float c = heightAndCone.y * heightAndCone.y / heightScale;

        float2 C = P + R * t;
        if (C.y <= h)
        {
            break;
        }

        float2 Q = float2(C.x, h);
        float2 S = normalize(float2(c, 1.0));
        float newT = dot(Q - P, float2(S.y, -S.x)) / dot(R, float2(S.y, -S.x));
        t = max(t + minStep, newT);
    }

    float2 texC = uv.xy + texDir3D.xy * t;
    return texC.xy;
}

public float2 RelaxedConeStepMapping(Texture2D coneMapTexture, SamplerState coneMapSampler, float heightScale, uint numLayers, float2 uv, float2 ddxUV, float2 ddyUV, float3 texDir3D)
{
    const uint binarySteps = 6;

    float3 rayPos = float3(uv, 0.0);
    float3 rayDir = GetRayDirection(texDir3D, heightScale);

    rayDir /= rayDir.z;
    float rayRatio = length(rayDir.xy);
    float3 pos = rayPos;
    for (uint i = 0; i < numLayers; ++i)
    {
        float2 heightAndCone = clamp(coneMapTexture.SampleGrad(coneMapSampler, pos.xy, ddxUV, ddyUV).rg, 0.0, 1.0);
        float coneRatio = heightAndCone.g * heightAndCone.g;
        float h = GetInverseHeight(heightAndCone.r) - pos.z;
        float d = coneRatio * h / (rayRatio + coneRatio);
        pos += rayDir * d;
    }

    // Binary search
    float3 bsRange = 0.5 * rayDir * pos.z;
    float3 bsPosition = rayPos + bsRange;
    for (uint j = 0; j < binarySteps; ++j)
    {
        // Sample at the position the binary search is converging on (bsPosition), not the fixed overshoot
        // point (pos) from the linear loop — matches the terrain cone-step shader's binary search.
        float2 heightAndCone = clamp(coneMapTexture.SampleGrad(coneMapSampler, bsPosition.xy, ddxUV, ddyUV).rg, 0.0, 1.0);
        bsRange *= 0.5;
        if (bsPosition.z < GetInverseHeight(heightAndCone.r))
        {
            bsPosition += bsRange;
        }
        else
        {
            bsPosition -= bsRange;
        }
    }
    return bsPosition.xy;
}

// Texture2DArray overload: samples the cone map from array `layer` (used by terrain, which blends multiple
// material layers). Same algorithm as the Texture2D version above - only the sample source differs.
public float2 RelaxedConeStepMapping(Texture2DArray coneMapTextures, SamplerState coneMapSampler, uint layer, float heightScale, uint numLayers, float2 uv, float2 ddxUV, float2 ddyUV, float3 texDir3D)
{
    const uint binarySteps = 6;

    float3 rayPos = float3(uv, 0.0);
    float3 rayDir = GetRayDirection(texDir3D, heightScale);

    rayDir /= rayDir.z;
    float rayRatio = length(rayDir.xy);
    float3 pos = rayPos;
    for (uint i = 0; i < numLayers; ++i)
    {
        float2 heightAndCone = clamp(coneMapTextures.SampleGrad(coneMapSampler, float3(pos.xy, (float)layer), ddxUV, ddyUV).rg, 0.0, 1.0);
        float coneRatio = heightAndCone.g * heightAndCone.g;
        float h = GetInverseHeight(heightAndCone.r) - pos.z;
        float d = coneRatio * h / (rayRatio + coneRatio);
        pos += rayDir * d;
    }

    // Binary search (samples the converging position bsPosition, not the linear overshoot pos).
    float3 bsRange = 0.5 * rayDir * pos.z;
    float3 bsPosition = rayPos + bsRange;
    for (uint j = 0; j < binarySteps; ++j)
    {
        float2 heightAndCone = clamp(coneMapTextures.SampleGrad(coneMapSampler, float3(bsPosition.xy, (float)layer), ddxUV, ddyUV).rg, 0.0, 1.0);
        bsRange *= 0.5;
        if (bsPosition.z < GetInverseHeight(heightAndCone.r))
        {
            bsPosition += bsRange;
        }
        else
        {
            bsPosition -= bsRange;
        }
    }
    return bsPosition.xy;
}
