// Clouds weather texture compute shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct CloudParams
{
    float4 outputTextureSize;
    float4 seed;
    float perlinAmplitude;
    float perlinFrequency;
    float perlinScale;
    int perlinOctaves;
};

[[vk::binding(0)]] ConstantBuffer<CloudParams> uboCS;
[[vk::binding(1), vk::image_format("rgba8")]] WTexture2D<float4> outWeatherTexture;

float Random2D(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233) + uboCS.seed.xy)) * 43758.5453123);
}

float NoiseInterpolation(float2 iCoord, float iSize)
{
    float2 grid = iCoord * iSize;
    float2 randomInput = floor(grid);

    float p0 = Random2D(randomInput);
    float p1 = Random2D(randomInput + float2(1.0, 0.0));
    float p2 = Random2D(randomInput + float2(0.0, 1.0));
    float p3 = Random2D(randomInput + float2(1.0, 1.0));

    float2 gridFraction = frac(grid);
    float2 weights = smoothstep(float2(0.0, 0.0), float2(1.0, 1.0), gridFraction);

    return p0 + (p1 - p0) * (weights.x) + (p2 - p0) * (weights.y) * (1.0 - weights.x) + (p3 - p1) * (weights.y * weights.x);
}

float PerlinNoise(float2 uv, float sc, float f, float a, int o)
{
    float noiseValue = 0.0;
    float localAmplitude = a;
    float localFrequency = f;
    for (int index = 0; index < o; ++index)
    {
        noiseValue += NoiseInterpolation(uv, sc * localFrequency) * localAmplitude;
        localAmplitude *= 0.25;
        localFrequency *= 3.0;
    }
    return noiseValue * noiseValue;
}

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchThreadID.xy);

    float2 uv = float2(pixel) / uboCS.outputTextureSize.xy;
    float2 suv = uv + float2(5.5, 5.5);

    float cloudType = clamp(PerlinNoise(suv, uboCS.perlinScale * 3.0, 0.3, 0.7, 10), 0.0, 1.0);

    float coverage = PerlinNoise(uv, uboCS.perlinScale, uboCS.perlinFrequency, uboCS.perlinAmplitude, uboCS.perlinOctaves);
    float4 weather = float4(clamp(coverage, 0, 1), cloudType, 0, 1);

    outWeatherTexture.Store(pixel, weather);
}
