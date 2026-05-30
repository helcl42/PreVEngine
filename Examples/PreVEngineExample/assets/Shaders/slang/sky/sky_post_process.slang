// Sky post-process compute shader (gaussian blur + god rays)
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SkyPostProcessParams
{
    float4 resolution;
    float4 lightPosition;
    uint enableGodRays;
    float lightDotCameraForward;
};

[[vk::binding(0), vk::image_format("rgba8")]] WTexture2D<float4> outFragColor;
[[vk::binding(1)]] Texture2D skyTex;
[[vk::binding(2)]] SamplerState skySampler;
[[vk::binding(3)]] Texture2D bloomTex;
[[vk::binding(4)]] SamplerState bloomSampler;
[[vk::binding(5)]] ConstantBuffer<SkyPostProcessParams> uboCS;

float4 GaussianBlur(Texture2D tex, SamplerState texSampler, float2 uv)
{
    float BLUR_OFFSET_X = 1.0 / uboCS.resolution.x;
    float BLUR_OFFSET_Y = 1.0 / uboCS.resolution.y;

    float2 offsets[9] = {
        float2(-BLUR_OFFSET_X,  BLUR_OFFSET_Y),
        float2(0.0, BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, 0.0),
        float2(0.0, 0.0),
        float2(BLUR_OFFSET_X, 0.0),
        float2(BLUR_OFFSET_X, -BLUR_OFFSET_Y),
        float2(0.0, -BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, -BLUR_OFFSET_Y)
    };

    float kernel[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    };

    float4 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = tex.SampleLevel(texSampler, uv + offsets[i], 0.0);
    }

    float4 col = float4(0.0);
    for (int j = 0; j < 9; j++)
    {
        col += sampleTex[j] * kernel[j];
    }
    return col;
}

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 fragCoord = int2(dispatchThreadID.xy);
    float2 uv = float2(fragCoord.xy) / uboCS.resolution.xy;

    float4 fragColor = GaussianBlur(skyTex, skySampler, uv);

    // RADIAL BLUR + CREPUSCULAR RAYS
    if (uboCS.lightDotCameraForward > 0.0 && uboCS.enableGodRays != 0)
    {
        float decay = 0.98;
        float density = 0.9;
        float weight = 0.07;
        float exposure = 0.45;

        const int SAMPLES = 64;
        float illuminationDecay = 1.0;

        float2 textureCoord = uv;
        float2 textureCoordDelta = textureCoord - uboCS.lightPosition.xy;
        textureCoordDelta *= density / float(SAMPLES);

        float3 colRays = GaussianBlur(bloomTex, bloomSampler, uv).rgb * 0.4;
        float2 sampleUv = uv;
        for (int i = 0; i < SAMPLES; i++)
        {
            sampleUv -= textureCoordDelta;
            colRays += bloomTex.SampleLevel(bloomSampler, sampleUv, 0.0).rgb * illuminationDecay * weight;
            illuminationDecay *= decay;
        }

        float3 colorWithRays = fragColor.rgb + (smoothstep(float3(0.0), float3(1.0), colRays) * exposure);
        fragColor.rgb = lerp(fragColor.rgb, colorWithRays * 0.9, uboCS.lightDotCameraForward * uboCS.lightDotCameraForward);
    }

    outFragColor.Store(fragCoord, fragColor);
}
