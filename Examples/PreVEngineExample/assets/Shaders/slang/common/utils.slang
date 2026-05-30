// Common utilities module
module utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

static const float PI = 3.1415927;
static const float PI_2 = 1.5707963;
static const float INV_PI = 0.3183098;
static const float INV_PI_2 = 0.636619772;

public float Log10(float x)
{
    return log2(x) / log2(10);
}

public float SquareLength(float2 v)
{
    return v.x * v.x + v.y * v.y;
}

public float SquareLength(float3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

public float Luminance(float3 rgb)
{
    const float3 kLum = float3(0.2126, 0.7152, 0.0722);
    return max(dot(rgb, kLum), 0.0001);
}

public float LinearizeDepth(float depth, float zNear, float zFar)
{
    return (zNear * zFar) / (zFar + depth * (zNear - zFar));
}

public float DelinearizeDepth(float linearDepth, float zNear, float zFar)
{
    return (((zNear * zFar) / linearDepth) - zFar) / (zNear - zFar);
}

public float2x2 MakeRotation(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    return float2x2(c, s, -s, c);
}

public float ComputeDepth(float4x4 projectionMatrix, float4x4 viewMatrix, float3 pointInWorldSpace)
{
    float4 pInClipSpace = mul(projectionMatrix, mul(viewMatrix, float4(pointInWorldSpace, 1.0)));
    float3 pInNDC = pInClipSpace.xyz / pInClipSpace.w;
    return pInNDC.z;
}
