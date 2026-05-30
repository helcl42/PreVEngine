// Volumetric clouds compute shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SkyCloudParams
{
    float4 resolution;
    float4x4 projectionMatrix;
    float4x4 inverseProjectionMatrix;
    float4x4 viewMatrix;
    float4x4 inverseViewMatrix;
    float4 lightColor;
    float4 lightDirection;
    float4 cameraPosition;
    float4 baseCloudColor;
    float4 skyColorBottom;
    float4 skyColorTop;
    float4 windDirection;
    float4 worldOrigin;
    float time;
    float coverageFactor;
    float cloudSpeed;
    float crispiness;
    float absorption;
    float curliness;
    uint enablePowder;
    float densityFactor;
    float earthRadius;
    float sphereInnerRadius;
    float sphereOuterRadius;
    float cloudTopOffset;
    float maxDepth;
};

[[vk::binding(0), vk::image_format("rgba8")]] WTexture2D<float4> outFragColor;
[[vk::binding(1), vk::image_format("rgba8")]] WTexture2D<float4> outBloom;
[[vk::binding(2), vk::image_format("rgba8")]] WTexture2D<float4> outAlphaness;
[[vk::binding(3), vk::image_format("r32f")]] WTexture2D<float4> outCloudDistance;
[[vk::binding(4)]] Texture3D perlinNoiseTex;
[[vk::binding(5)]] SamplerState perlinNoiseSampler;
[[vk::binding(6)]] Texture2D weatherTex;
[[vk::binding(7)]] SamplerState weatherSampler;
[[vk::binding(8)]] ConstantBuffer<SkyCloudParams> uboCS;

// Cone sampling random offsets
static const float3 noiseKernel[6] = {
    float3( 0.38051305,  0.92453449, -0.02111345),
    float3(-0.50625799, -0.03590792, -0.86163418),
    float3(-0.32509218, -0.94557439,  0.01428793),
    float3( 0.09026238, -0.27376545,  0.95755165),
    float3( 0.28128598,  0.42443639, -0.86065785),
    float3(-0.16852403,  0.14748697,  0.97460106)
};

static const float bayerFilter[16] = {
    0.0 / 16.0, 8.0 / 16.0, 2.0 / 16.0, 10.0 / 16.0,
    12.0 / 16.0, 4.0 / 16.0, 14.0 / 16.0, 6.0 / 16.0,
    3.0 / 16.0, 11.0 / 16.0, 1.0 / 16.0, 9.0 / 16.0,
    15.0 / 16.0, 7.0 / 16.0, 13.0 / 16.0, 5.0 / 16.0
};

// Cloud types height density gradients
static const float4 statusGradient = float4(0.0, 0.1, 0.2, 0.3);
static const float4 stratoCumusGradient = float4(0.02, 0.2, 0.48, 0.625);
static const float4 cumulusGradient = float4(0.00, 0.1625, 0.88, 0.98);

static const float cloudsMinTransmitance = 0.1;

float3 GetEarthCenter()
{
    return uboCS.worldOrigin.xyz + float3(0.0, -uboCS.earthRadius, 0.0);
}

float HG(float sunDotRayDirection, float g)
{
    float gg = g * g;
    return (1.0 - gg) / pow(1.0 + gg - 2.0 * g * sunDotRayDirection, 1.5);
}

bool RaySphereIntersection(float3 rayStartPosition, float3 rayDirection, float3 sphereCenter, float radius, out float3 intersectionPoint)
{
    float radius2 = radius * radius;
    float3 L = rayStartPosition - sphereCenter;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(rayDirection, L);
    float c = dot(L, L) - radius2;

    float discr = b * b - 4.0 * a * c;
    if (discr < 0.0)
    {
        intersectionPoint = float3(0.0);
        return false;
    }

    float sqrtDiscr = sqrt(discr);
    float t1 = (-b - sqrtDiscr) / (2.0 * a);
    float t2 = (-b + sqrtDiscr) / (2.0 * a);

    float t = (t1 > 0.0) ? t1 : t2;
    if (t <= 0.0)
    {
        intersectionPoint = float3(0.0);
        return false;
    }

    intersectionPoint = rayStartPosition + rayDirection * t;
    return true;
}

float GetHeightFraction(float3 inPos)
{
    float3 sphereCenter = GetEarthCenter();
    return (length(inPos - sphereCenter) - uboCS.sphereInnerRadius) / (uboCS.sphereOuterRadius - uboCS.sphereInnerRadius);
}

float Remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
{
    return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

float GetDensityForCloud(float heightFraction, float cloudType)
{
    float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
    float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
    float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

    float4 baseGradient = stratusFactor * statusGradient + stratoCumulusFactor * stratoCumusGradient + cumulusFactor * cumulusGradient;

    return smoothstep(baseGradient.x, baseGradient.y, heightFraction) - smoothstep(baseGradient.z, baseGradient.w, heightFraction);
}

float2 GetUVProjection(float3 inPos)
{
    return inPos.xz / uboCS.sphereInnerRadius + 0.5;
}

float SampleCloudDensity(float3 inPos, bool expensive, float lod)
{
    float heightFraction = GetHeightFraction(inPos);
    float3 animation = heightFraction * uboCS.windDirection.xyz * uboCS.cloudTopOffset + uboCS.windDirection.xyz * uboCS.time * uboCS.cloudSpeed;
    float2 uv = GetUVProjection(inPos);
    float2 movingUV = GetUVProjection(inPos + animation);

    if (heightFraction < 0.0 || heightFraction > 1.0)
    {
        return 0.0;
    }

    float3 noiseScaler = float3(0.625, 0.25, 0.125);
    float4 lowFrequencyNoise = perlinNoiseTex.SampleLevel(perlinNoiseSampler, float3(uv * uboCS.crispiness, heightFraction), lod);
    float lowFreqFBM = dot(lowFrequencyNoise.gba, noiseScaler);
    float baseCloud = Remap(lowFrequencyNoise.r, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);

    float density = GetDensityForCloud(heightFraction, 1.0);
    baseCloud *= density / max(heightFraction, 0.001);

    float3 weatherData = weatherTex.SampleLevel(weatherSampler, movingUV, 0.0).rgb;
    float cloudCoverage = weatherData.r * uboCS.coverageFactor;
    float baseCloudWithCoverage = Remap(baseCloud, cloudCoverage, 1.0, 0.0, 1.0);
    baseCloudWithCoverage *= cloudCoverage;

    if (expensive)
    {
        float3 erodeCloudNoise = perlinNoiseTex.SampleLevel(perlinNoiseSampler, float3(movingUV * uboCS.crispiness, heightFraction) * uboCS.curliness, lod).rgb;
        float highFreqFBM = dot(erodeCloudNoise.rgb, noiseScaler);
        float highFreqNoiseModifier = lerp(highFreqFBM, 1.0 - highFreqFBM, clamp(heightFraction * 10.0, 0.0, 1.0));

        baseCloudWithCoverage = baseCloudWithCoverage - highFreqNoiseModifier * (1.0 - baseCloudWithCoverage);
        baseCloudWithCoverage = Remap(baseCloudWithCoverage * 2.0, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);
    }

    return clamp(baseCloudWithCoverage, 0.0, 1.0);
}

float GetPowder(float density)
{
    return (1.0 - exp(-2.0 * density));
}

float RaymarchToLight(float3 inPos, float stepSize, float3 lightDir, float originalDensity, float lightDotEye)
{
    float ds = stepSize * 6.0;
    float3 rayStep = lightDir * ds;
    float coneStep = 1.0 / 6.0;
    float sigmaDs = -ds * uboCS.absorption;

    float coneRadius = 1.0;
    float density = 0.0;
    float3 startPos = inPos;
    float T = 1.0;
    for (int i = 0; i < 6; i++)
    {
        float3 pos = startPos + coneRadius * noiseKernel[i] * float(i);
        float heightFraction = GetHeightFraction(pos);
        if (heightFraction >= 0)
        {
            float cloudDensity = SampleCloudDensity(pos, density > 0.3, i / 16);
            if (cloudDensity > 0.0)
            {
                float Ti = exp(cloudDensity * sigmaDs);
                T *= Ti;
                density += cloudDensity;
            }
        }
        startPos += rayStep;
        coneRadius += coneStep;
    }
    return T;
}

struct RaymarchToCloudResult
{
    bool hit;
    float3 position;
    float4 color;
};

RaymarchToCloudResult RaymarchToCloud(float3 startPos, float3 endPos, float3 bg, uint2 fragCoord)
{
    const int countOfSteps = 64;
    float3 path = endPos - startPos;
    float pathLength = length(path);
    float stepVal = pathLength / countOfSteps;
    float3 dir = normalize(path);
    float3 stepVector = dir * stepVal;

    int a = int(fragCoord.x) % 4;
    int b = int(fragCoord.y) % 4;
    float3 pos = startPos + stepVector * bayerFilter[a * 4 + b];
    float density = 0.0;
    float lightDotEye = dot(normalize(uboCS.lightDirection.xyz), dir);

    float T = 1.0;
    float sigmaDs = -stepVal * uboCS.densityFactor;

    RaymarchToCloudResult result;
    result.hit = false;
    result.position = float3(1e12, 1e12, 1e12);
    result.color = float4(0.0);

    for (int i = 0; i < countOfSteps; i++)
    {
        float densitySample = SampleCloudDensity(pos, true, i / 16);
        if (densitySample > 0.0)
        {
            if (!result.hit)
            {
                result.position = pos;
                result.hit = true;
            }

            float3 ambientLight = uboCS.baseCloudColor.xyz;
            float lightDensity = RaymarchToLight(pos, stepVal * 0.1, uboCS.lightDirection.xyz, densitySample, lightDotEye);
            float scattering = lerp(HG(lightDotEye, -0.08), HG(lightDotEye, 0.08), clamp(lightDotEye * 0.5 + 0.5, 0.0, 1.0));
            scattering = max(scattering, 1.0);
            float powderTerm = 1.0;
            if (uboCS.enablePowder == 1)
            {
                powderTerm = GetPowder(densitySample);
            }
            float3 S = 0.6 * lerp(lerp(ambientLight * 1.8, bg, 0.2), scattering * uboCS.lightColor.rgb, powderTerm * lightDensity) * densitySample;
            float dTrans = exp(densitySample * sigmaDs);
            float3 Sint = (S - S * dTrans) * (1.0 / densitySample);
            result.color.rgb += T * Sint;
            T *= dTrans;
        }

        if (T <= cloudsMinTransmitance)
        {
            break;
        }

        pos += stepVector;
    }

    result.color.a = 1.0 - T;
    return result;
}

float ComputeFogAmount(float3 startPos, float factor)
{
    float3 sphereCenter = GetEarthCenter();
    float dist = length(startPos - uboCS.cameraPosition.xyz);
    float radius = (uboCS.cameraPosition.y - sphereCenter.y) * 0.3;
    float alpha = dist / radius;
    return 1.0 - exp(-dist * alpha * factor);
}

float3 GetSunColor(float3 worldRayDirection, float powExp)
{
    float3 sunColor = float3(1.0, 0.6, 0.1);
    float sun = clamp(dot(uboCS.lightDirection.xyz, worldRayDirection), 0.0, 1.0);
    return 0.8 * sunColor * pow(sun, powExp);
}

float3 GetCubeMapColor(float3 worldRayDirection)
{
    float transitionSharpness = 0.07;
    float transitionRatio = smoothstep(-transitionSharpness, transitionSharpness, worldRayDirection.y);
    return lerp(uboCS.skyColorBottom.xyz, uboCS.skyColorTop.xyz, transitionRatio);
}

float4 GetBackgroundColor(float3 worldRayDirection)
{
    float3 backgroundColor = GetCubeMapColor(worldRayDirection);
    float3 sunColor = GetSunColor(worldRayDirection, 350.0);
    return float4(backgroundColor + sunColor, 1.0);
}

float4 ComputeClipSpaceDirection(int2 fragCoord)
{
    float2 normalizedFragCoord = float2(fragCoord.xy) / uboCS.resolution.xy;
    float2 rayNds = 2.0 * normalizedFragCoord - 1.0;
    return float4(rayNds, 1.0, 1.0);
}

float4 ComputeViewSpaceDirection(float4 clipSpaceDir)
{
    float4 rayView = mul(uboCS.inverseProjectionMatrix, clipSpaceDir);
    return float4(rayView.xy, -1.0, 0.0);
}

float3 ComputeWorldSpaceDirection(float4 viewSpaceDir)
{
    float3 worldDir = mul(uboCS.inverseViewMatrix, viewSpaceDir).xyz;
    return normalize(worldDir);
}

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 fragCoord = int2(dispatchThreadID.xy);

    float4 clipDir = ComputeClipSpaceDirection(fragCoord);
    float4 viewDir = ComputeViewSpaceDirection(clipDir);
    float3 worldDir = ComputeWorldSpaceDirection(viewDir);

    float4 backgroundColor = GetBackgroundColor(worldDir);

    float3 sphereCenter = GetEarthCenter();

    float3 startPos, endPos, fogRay;
    float camDistFromCenter = length(uboCS.cameraPosition.xyz - sphereCenter);
    if (camDistFromCenter < uboCS.sphereInnerRadius)
    {
        RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereInnerRadius, startPos);
        RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereOuterRadius, endPos);
        fogRay = startPos;
    }
    else if (camDistFromCenter >= uboCS.sphereInnerRadius && camDistFromCenter < uboCS.sphereOuterRadius)
    {
        startPos = uboCS.cameraPosition.xyz;
        float3 innerHit;
        bool hitInner = RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereInnerRadius, innerHit);
        if (hitInner)
        {
            endPos = innerHit;
            fogRay = innerHit;
        }
        else
        {
            RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereOuterRadius, endPos);
            fogRay = startPos;
        }
    }
    else
    {
        RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereOuterRadius, startPos);
        RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereInnerRadius, endPos);
        fogRay = startPos;
    }

    float fogAmount = ComputeFogAmount(fogRay, 0.00006);
    if (fogAmount > 0.965)
    {
        outFragColor.Store(fragCoord, backgroundColor);
        outBloom.Store(fragCoord, backgroundColor);
        outAlphaness.Store(fragCoord, float4(0.0));
        outCloudDistance.Store(fragCoord, float4(uboCS.maxDepth, 0.0, 0.0, 0.0));
        return;
    }

    RaymarchToCloudResult raymarchResult = RaymarchToCloud(startPos, endPos, backgroundColor.rgb, uint2(fragCoord));

    raymarchResult.color.rgb = raymarchResult.color.rgb * 1.8 - 0.1;
    raymarchResult.color.rgb = lerp(raymarchResult.color.rgb, backgroundColor.rgb * raymarchResult.color.a, clamp(fogAmount, 0.0, 1.0));

    float sun = clamp(dot(uboCS.lightDirection.xyz, normalize(endPos - startPos)), 0.0, 1.0);
    float3 s = 0.8 * float3(1.0, 0.4, 0.2) * pow(sun, 256.0);
    raymarchResult.color.rgb += s * raymarchResult.color.a;

    backgroundColor.rgb = backgroundColor.rgb * (1.0 - raymarchResult.color.a) + raymarchResult.color.rgb;
    backgroundColor.a = 1.0;

    float4 bloomColor = float4(GetSunColor(worldDir, 128) * 1.3, 1.0);

    float cloudAlphaness = clamp(raymarchResult.color.a, 0.2, 1.0);
    float4 alphaness = float4(cloudAlphaness, 0.0, 0.0, 1.0);
    if (cloudAlphaness > 0.1)
    {
        float fogAmt = ComputeFogAmount(startPos, 0.00003);
        float3 cloud = lerp(float3(0.0), bloomColor.rgb, clamp(fogAmt, 0.0, 1.0));
        bloomColor.rgb = bloomColor.rgb * (1.0 - cloudAlphaness) + cloud.rgb;
    }

    float4 fragColor = float4(backgroundColor.rgb, 1.0);
    float4 depth = float4(ComputeDepth(uboCS.projectionMatrix, uboCS.viewMatrix, raymarchResult.position), 0.0, 0.0, 0.0);

    outFragColor.Store(fragCoord, fragColor);
    outBloom.Store(fragCoord, bloomColor);
    outAlphaness.Store(fragCoord, alphaness);
    outCloudDistance.Store(fragCoord, depth);
}
