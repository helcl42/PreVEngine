// Normal mapping module
module normal_mapping;

public float3 NormalMapping(Texture2D normalMapTexture, SamplerState normalMapSampler, float2 uv)
{
    return normalize(2.0 * normalMapTexture.Sample(normalMapSampler, uv).xyz - 1.0);
}

public float3 NormalMappingGrad(Texture2D normalMapTexture, SamplerState normalMapSampler, float2 uv, float2 ddxUV, float2 ddyUV)
{
    return normalize(2.0 * normalMapTexture.SampleGrad(normalMapSampler, uv, ddxUV, ddyUV).xyz - 1.0);
}
