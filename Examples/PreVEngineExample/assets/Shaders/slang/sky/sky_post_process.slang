// Sky post-process compute shader (background compositing + gaussian blur + god rays)
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SkyPostProcessParams
{
    float4 resolution;
    float4 lightPosition;
    float4x4 inverseProjectionMatrix;
    float4x4 inverseViewMatrix;
    float4 lightDirection;
    float4 skyColorBottom;
    float4 skyColorTop;
    uint enableGodRays;
    uint enableBlur;
    float lightDotCameraForward;
    uint godRaySamples;
    uint enableCheckerboardResolve;
    uint frameCounter;
    float pad0;
    float pad1;
};

[[vk::binding(0), vk::image_format("rgba8")]] WTexture2D<float4> outFragColor;
[[vk::binding(1)]] Texture2D skyTex;
[[vk::binding(2)]] SamplerState skySampler;
[[vk::binding(3)]] Texture2D bloomTex;
[[vk::binding(4)]] SamplerState bloomSampler;
[[vk::binding(5)]] ConstantBuffer<SkyPostProcessParams> uboCS;

float3 GetWorldDirection(float2 uv)
{
    float2 ndc = uv * 2.0 - 1.0;
    float4 clipDir = float4(ndc, 1.0, 1.0);
    float4 viewDir = mul(uboCS.inverseProjectionMatrix, clipDir);
    viewDir = float4(viewDir.xy, -1.0, 0.0);
    float3 worldDir = mul(uboCS.inverseViewMatrix, viewDir).xyz;
    return normalize(worldDir);
}

float3 GetSunColor(float3 worldRayDirection, float powExp)
{
    float3 sunColor = float3(1.0, 0.6, 0.1);
    float sun = clamp(dot(uboCS.lightDirection.xyz, worldRayDirection), 0.0, 1.0);
    return 0.8 * sunColor * pow(sun, powExp);
}

float4 GetBackgroundColor(float3 worldRayDirection)
{
    float transitionSharpness = 0.07;
    float transitionRatio = smoothstep(-transitionSharpness, transitionSharpness, worldRayDirection.y);
    float3 backgroundColor = lerp(uboCS.skyColorBottom.xyz, uboCS.skyColorTop.xyz, transitionRatio);
    float3 sunColor = GetSunColor(worldRayDirection, 350.0);
    return float4(backgroundColor + sunColor, 1.0);
}

// Composite cloud data with background, then sample
float4 CompositeAndSample(Texture2D tex, SamplerState texSampler, float2 uv)
{
    float4 cloudData = tex.SampleLevel(texSampler, uv, 0.0);
    float3 worldDir = GetWorldDirection(uv);
    float4 bg = GetBackgroundColor(worldDir);
    float3 composited = bg.rgb * (1.0 - cloudData.a) + cloudData.rgb;
    return float4(composited, 1.0);
}

float4 GaussianBlurComposited(Texture2D tex, SamplerState texSampler, float2 uv)
{
    float BLUR_OFFSET_X = 1.0 / uboCS.resolution.x;
    float BLUR_OFFSET_Y = 1.0 / uboCS.resolution.y;

    float2 offsets[9] = {
        float2(-BLUR_OFFSET_X,  BLUR_OFFSET_Y),
        float2(0.0, BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, BLUR_OFFSET_Y),
        float2(-BLUR_OFFSET_X, 0.0),
        float2(0.0, 0.0),
        float2(BLUR_OFFSET_X, 0.0),
        float2(-BLUR_OFFSET_X, -BLUR_OFFSET_Y),
        float2(0.0, -BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, -BLUR_OFFSET_Y)
    };

    float kernel[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    };

    float4 col = float4(0.0);
    for (int i = 0; i < 9; i++)
    {
        col += CompositeAndSample(tex, texSampler, uv + offsets[i]) * kernel[i];
    }
    return col;
}

float4 CheckerboardResolve(Texture2D tex, SamplerState texSampler, int2 fragCoord, float2 uv)
{
    bool isReprojected = ((fragCoord.x + fragCoord.y) & 1) != int(uboCS.frameCounter & 1);
    if (isReprojected)
    {
        float2 texelSize = 1.0 / uboCS.resolution.xy;
        float4 c0 = CompositeAndSample(tex, texSampler, uv + float2(-texelSize.x, -texelSize.y));
        float4 c1 = CompositeAndSample(tex, texSampler, uv + float2( texelSize.x, -texelSize.y));
        float4 c2 = CompositeAndSample(tex, texSampler, uv + float2(-texelSize.x,  texelSize.y));
        float4 c3 = CompositeAndSample(tex, texSampler, uv + float2( texelSize.x,  texelSize.y));
        float4 center = CompositeAndSample(tex, texSampler, uv);
        return lerp(center, (c0 + c1 + c2 + c3) * 0.25, 0.5);
    }
    return CompositeAndSample(tex, texSampler, uv);
}

float4 GaussianBlur(Texture2D tex, SamplerState texSampler, float2 uv)
{
    float BLUR_OFFSET_X = 1.0 / uboCS.resolution.x;
    float BLUR_OFFSET_Y = 1.0 / uboCS.resolution.y;

    float2 offsets[9] = {
        float2(-BLUR_OFFSET_X,  BLUR_OFFSET_Y),
        float2(0.0, BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, BLUR_OFFSET_Y),
        float2(-BLUR_OFFSET_X, 0.0),
        float2(0.0, 0.0),
        float2(BLUR_OFFSET_X, 0.0),
        float2(-BLUR_OFFSET_X, -BLUR_OFFSET_Y),
        float2(0.0, -BLUR_OFFSET_Y),
        float2(BLUR_OFFSET_X, -BLUR_OFFSET_Y)
    };

    float kernel[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    };

    float4 col = float4(0.0);
    for (int i = 0; i < 9; i++)
    {
        col += tex.SampleLevel(texSampler, uv + offsets[i], 0.0) * kernel[i];
    }
    return col;
}

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 fragCoord = int2(dispatchThreadID.xy);
    float2 uv = float2(fragCoord.xy) / uboCS.resolution.xy;

    // Composite cloud data with fresh background
    float4 fragColor;
    if (uboCS.enableBlur != 0)
    {
        fragColor = GaussianBlurComposited(skyTex, skySampler, uv);
    }
    else if (uboCS.enableCheckerboardResolve != 0)
    {
        fragColor = CheckerboardResolve(skyTex, skySampler, fragCoord, uv);
    }
    else
    {
        fragColor = CompositeAndSample(skyTex, skySampler, uv);
    }

    // RADIAL BLUR + CREPUSCULAR RAYS
    if (uboCS.lightDotCameraForward > 0.0 && uboCS.enableGodRays != 0)
    {
        float decay = 0.98;
        float density = 0.9;
        float weight = 0.07;
        float exposure = 0.45;

        const int SAMPLES = int(uboCS.godRaySamples);
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
