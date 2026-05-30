// Shadows module
module shadows;

public static const bool enablePCF = true;
public static const float defaultShadowFactor = 0.2;
public static const float cascadeBiasDamper = 1.5;
public static const uint SHADOW_MAP_CASCADE_COUNT = 4;

public struct ShadowsCascade
{
    public float4x4 viewProjectionMatrix;
    public float4 split;
};

public struct Shadows
{
    public ShadowsCascade cascades[SHADOW_MAP_CASCADE_COUNT];
    public uint enabled;
    public uint useReverseDepth;
};

float GetShadowRawInternal(Texture2DArray depthTexture, SamplerState depthSampler, float4 shadowCoord, float2 shadowCoordOffset, uint cascadeIndex, float depthBias, uint useReverseDepth)
{
    float shadow = 1.0;
    if (shadowCoord.z >= 0.0 && shadowCoord.z <= 1.0)
    {
        float depth = depthTexture.SampleLevel(depthSampler, float3(shadowCoord.xy + shadowCoordOffset, (float)cascadeIndex), 0).r;
        if (useReverseDepth != 0)
        {
            if (depth > shadowCoord.z + depthBias)
            {
                shadow = defaultShadowFactor;
            }
        }
        else
        {
            if (depth < shadowCoord.z - depthBias)
            {
                shadow = defaultShadowFactor;
            }
        }
    }
    return shadow;
}

float GetShadowPCFInternal(Texture2DArray depthTexture, SamplerState depthSampler, float4 shadowCoord, uint cascadeIndex, float depthBias, uint useReverseDepth, float4 fragCoord)
{
    uint width, height, elements;
    depthTexture.GetDimensions(width, height, elements);
    float2 textureDim = float2((float)width, (float)height);
    float2 texelSize = 1.0 / textureDim;

    float shadow = 0.0;

    // Optimized PCF impl -> 4 samples
    float2 offset = frac(fragCoord.xy * 0.5) + 0.25;
    if (offset.y > 1.1)
    {
        offset.y = 0;
    }
    shadow = (
                GetShadowRawInternal(depthTexture, depthSampler, shadowCoord, texelSize * (offset + float2(-1.5, 0.5)), cascadeIndex, depthBias, useReverseDepth)
                + GetShadowRawInternal(depthTexture, depthSampler, shadowCoord, texelSize * (offset + float2(0.5, 0.5)), cascadeIndex, depthBias, useReverseDepth)
                + GetShadowRawInternal(depthTexture, depthSampler, shadowCoord, texelSize * (offset + float2(-1.5, -1.5)), cascadeIndex, depthBias, useReverseDepth)
                + GetShadowRawInternal(depthTexture, depthSampler, shadowCoord, texelSize * (offset + float2(0.5, -1.5)), cascadeIndex, depthBias, useReverseDepth)
                ) * 0.25;
    return shadow;
}

public float GetShadowSingle(Texture2DArray depthTexture, SamplerState depthSampler, float4 shadowCoord, uint cascadeIndex, float depthBias, uint useReverseDepth, float4 fragCoord)
{
    float shadow = 1.0;
    if (enablePCF)
    {
        shadow = GetShadowPCFInternal(depthTexture, depthSampler, shadowCoord, cascadeIndex, depthBias, useReverseDepth, fragCoord);
    }
    else
    {
        shadow = GetShadowRawInternal(depthTexture, depthSampler, shadowCoord, float2(0.0), cascadeIndex, depthBias, useReverseDepth);
    }
    return shadow;
}

public float GetShadow(Texture2DArray depthTexture, SamplerState depthSampler, Shadows shadows, float3 viewPosition, float3 worldPosition, float depthBias, float4 fragCoord)
{
    float shadow = 1.0;
    float bias = depthBias;
    if (shadows.enabled != 0)
    {
        uint cascadeIndex = 0;
        for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
        {
            if (shadows.useReverseDepth != 0)
            {
                if (viewPosition.z > shadows.cascades[i].split.x)
                {
                    cascadeIndex = i + 1;
                    bias /= cascadeBiasDamper;
                }
            }
            else
            {
                if (viewPosition.z < shadows.cascades[i].split.x)
                {
                    cascadeIndex = i + 1;
                    bias /= cascadeBiasDamper;
                }
            }
        }
        float4 shadowCoord = mul(shadows.cascades[cascadeIndex].viewProjectionMatrix, float4(worldPosition, 1.0));
        float4 normalizedShadowCoord = shadowCoord / shadowCoord.w;
        shadow = GetShadowSingle(depthTexture, depthSampler, normalizedShadowCoord, cascadeIndex, bias, shadows.useReverseDepth, fragCoord);
    }
    return shadow;
}
