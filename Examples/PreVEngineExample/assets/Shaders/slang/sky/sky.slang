// Volumetric clouds compute shader
import common.utils;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SkyCloudParams
{
    // View & Matrices
    float4 resolution;
    float4x4 projectionMatrix;
    float4x4 inverseProjectionMatrix;
    float4x4 viewMatrix;
    float4x4 inverseViewMatrix;

    // Light & Color
    float4 lightColor;
    float4 lightDirection;
    float4 baseCloudColor;
    float4 skyColorBottom;
    float4 skyColorTop;

    // Camera & World
    float4 cameraPosition;
    float4 worldOrigin;
    float4 windDirection;
    float earthRadius;
    float sphereInnerRadius;
    float sphereOuterRadius;
    float cloudTopOffset;

    // Cloud Shape
    float coverageFactor;
    float crispiness;
    float curliness;
    float densityFactor;
    float cloudSpeed;
    float absorption;
    float maxDepth;
    float time;

    // Rendering Quality
    uint cloudSteps;
    uint lightSteps;
    uint useIGN;
    uint frameCounter;

    // Effects
    uint enablePowder;
    float powderWeight;
    float lodScale;
    float ambientScale;

    // Reprojection
    float4x4 prevViewProjectionMatrix;
    float4x4 currentViewProjectionMatrix;
    float4x4 prevInverseViewProjectionMatrix;
    uint enableCheckerboard;
    float reprojectionBlend;
    uint enableTemporalBlend;
    uint enableFullReproject;
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
[[vk::binding(9)]] Texture2D prevColorTex;
[[vk::binding(10)]] SamplerState prevColorSampler;
[[vk::binding(11)]] Texture2D prevDepthTex;
[[vk::binding(12)]] SamplerState prevDepthSampler;

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

float BayerDither(uint2 fragCoord)
{
    int a = int(fragCoord.x) % 4;
    int b = int(fragCoord.y) % 4;
    return bayerFilter[a * 4 + b];
}

float InterleavedGradientNoise(float2 fragCoord)
{
    float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(fragCoord, magic.xy)));
}

float GetDitherOffset(uint2 fragCoord)
{
    if (uboCS.useIGN != 0)
    {
        return InterleavedGradientNoise(float2(fragCoord));
    }
    else
    {
        return BayerDither(fragCoord);
    }
}

// Cloud types height density gradients
static const float4 stratusGradient = float4(0.0, 0.1, 0.2, 0.3);
static const float4 stratoCumulusGradient = float4(0.02, 0.2, 0.48, 0.625);
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

    float4 baseGradient = stratusFactor * stratusGradient + stratoCumulusFactor * stratoCumulusGradient + cumulusFactor * cumulusGradient;

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
    // Base noise: static world position — fixed 3D noise field
    float4 lowFrequencyNoise = perlinNoiseTex.SampleLevel(perlinNoiseSampler, float3(uv * uboCS.crispiness, heightFraction), lod);
    float lowFreqFBM = dot(lowFrequencyNoise.gba, noiseScaler);
    float baseCloud = Remap(lowFrequencyNoise.r, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);

    float density = GetDensityForCloud(heightFraction, 1.0);
    baseCloud *= density / max(heightFraction, 0.001);

    // Weather moves with wind — slides over static noise field → natural evolution
    float3 weatherData = weatherTex.SampleLevel(weatherSampler, movingUV, 0.0).rgb;
    float cloudCoverage = weatherData.r * uboCS.coverageFactor;
    float baseCloudWithCoverage = Remap(baseCloud, cloudCoverage, 1.0, 0.0, 1.0);
    baseCloudWithCoverage *= cloudCoverage;

    if (expensive)
    {
        // Erosion moves with wind — detail drifts across cloud edges
        float3 erodeCloudNoise = perlinNoiseTex.SampleLevel(perlinNoiseSampler, float3(movingUV * uboCS.crispiness * uboCS.curliness, heightFraction), lod).rgb;
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
    int lightSteps = int(uboCS.lightSteps);
    float ds = stepSize * float(lightSteps);
    float3 rayStep = lightDir * ds;
    float coneStep = 1.0 / float(lightSteps);
    float sigmaDs = -ds * uboCS.absorption;

    float coneRadius = 1.0;
    float density = 0.0;
    float3 startPos = inPos;
    float T = 1.0;
    for (int i = 0; i < lightSteps; i++)
    {
        float3 pos = startPos + coneRadius * noiseKernel[i % 6] * float(i);
        float heightFraction = GetHeightFraction(pos);
        if (heightFraction >= 0)
        {
            float cloudDensity = SampleCloudDensity(pos, density > 0.3, float(i) / float(lightSteps));
            if (cloudDensity > 0.0)
            {
                float Ti = exp(cloudDensity * sigmaDs);
                T *= Ti;
                density += cloudDensity;
            }
        }
        if (T < 0.01)
        {
            break;
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
    int countOfSteps = int(uboCS.cloudSteps);
    float3 path = endPos - startPos;
    float pathLength = length(path);
    float stepVal = pathLength / countOfSteps;
    float3 dir = normalize(path);
    float3 stepVector = dir * stepVal;

    float3 pos = startPos + stepVector * GetDitherOffset(fragCoord);
    float density = 0.0;
    float lightDotEye = dot(normalize(uboCS.lightDirection.xyz), dir);

    float T = 1.0;
    float sigmaDs = -stepVal * uboCS.densityFactor;

    RaymarchToCloudResult result;
    result.hit = false;
    result.position = float3(1e12, 1e12, 1e12);
    result.color = float4(0.0);

    float cloudLayerThickness = uboCS.sphereOuterRadius - uboCS.sphereInnerRadius;
    for (int i = 0; i < countOfSteps; i++)
    {
        float distFromStart = float(i) * stepVal;
        // LOD 0 at ray start, ramps based on lodScale (higher = faster LOD falloff = less detail)
        float lod = clamp(distFromStart / (cloudLayerThickness * uboCS.lodScale), 0.0, 1.0);
        float densitySample = SampleCloudDensity(pos, true, lod);
        if (densitySample > 0.0)
        {
            if (!result.hit)
            {
                result.position = pos;
                result.hit = true;
            }

            float3 ambientLight = uboCS.baseCloudColor.xyz;
            // Ambient occlusion from height: darker at base, brighter at top
            float heightFrac = GetHeightFraction(pos);
            ambientLight *= lerp(0.8, 1.0, heightFrac);
            ambientLight *= uboCS.ambientScale;

            float lightDensity = RaymarchToLight(pos, stepVal * 0.1, uboCS.lightDirection.xyz, densitySample, lightDotEye);
            // Multi-scattering approximation: fake second bounce brightens deep cloud interiors
            float multiScatter = 1.0 - 0.5 * exp(-densitySample * 2.0);
            lightDensity = max(lightDensity, multiScatter * 0.15);

            float scattering = lerp(HG(lightDotEye, -0.08), HG(lightDotEye, 0.08), clamp(lightDotEye * 0.5 + 0.5, 0.0, 1.0));
            scattering = max(scattering, 0.035);
            // Beer-powder: subtle darkening of forward-scattering edges near sun
            float powderTerm = 1.0;
            if (uboCS.enablePowder == 1)
            {
                float powder = GetPowder(densitySample);
                // Only apply in forward-scattering direction (looking toward sun)
                float sunFacing = clamp(lightDotEye * 0.5 + 0.5, 0.0, 1.0);
                powderTerm = 1.0 - sunFacing * (1.0 - powder) * uboCS.powderWeight;
            }
            float3 S = 0.7 * lerp(lerp(ambientLight, bg, 0.2), scattering * uboCS.lightColor.rgb, lightDensity) * densitySample * powderTerm;
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
    if (camDistFromCenter < uboCS.sphereOuterRadius)
    {
        startPos = uboCS.cameraPosition.xyz;
        RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereOuterRadius, endPos);
        if (camDistFromCenter < uboCS.sphereInnerRadius)
        {
            RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereInnerRadius, fogRay);
            // Advance startPos toward the cloud shell to preserve sampling quality.
            // Blend between camera and inner sphere entry based on altitude so there's no jump.
            float distBelowInner = uboCS.sphereInnerRadius - camDistFromCenter;
            float fadeRange = (uboCS.sphereOuterRadius - uboCS.sphereInnerRadius) * 0.1;
            float blendFactor = clamp(distBelowInner / fadeRange, 0.0, 1.0);
            startPos = lerp(uboCS.cameraPosition.xyz, fogRay, blendFactor);
        }
        else
        {
            fogRay = uboCS.cameraPosition.xyz;
        }
    }
    else
    {
        bool hitOuter = RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereOuterRadius, startPos);
        if (!hitOuter)
        {
            outFragColor.Store(fragCoord, float4(0.0));
            outBloom.Store(fragCoord, float4(GetSunColor(worldDir, 128) * 1.3, 1.0));
            outAlphaness.Store(fragCoord, float4(0.0));
            outCloudDistance.Store(fragCoord, float4(uboCS.maxDepth, 0.0, 0.0, 0.0));
            return;
        }

        bool hitInner = RaySphereIntersection(uboCS.cameraPosition.xyz, worldDir, sphereCenter, uboCS.sphereInnerRadius, endPos);
        if (!hitInner)
        {
            float3 L = uboCS.cameraPosition.xyz - sphereCenter;
            float a = dot(worldDir, worldDir);
            float b = 2.0 * dot(worldDir, L);
            float c = dot(L, L) - uboCS.sphereOuterRadius * uboCS.sphereOuterRadius;
            float discr = b * b - 4.0 * a * c;
            float t2 = (-b + sqrt(discr)) / (2.0 * a);
            endPos = uboCS.cameraPosition.xyz + worldDir * t2;
        }
        fogRay = startPos;
    }

    float fogAmount = ComputeFogAmount(fogRay, 0.00006);
    // Gradually reduce fog as camera approaches inner sphere so there's no sudden change
    // when entering the shell. Fade happens over the last 10% of the distance to inner sphere.
    if (camDistFromCenter < uboCS.sphereOuterRadius)
    {
        float distBelowInner = uboCS.sphereInnerRadius - camDistFromCenter;
        float fadeRange = (uboCS.sphereOuterRadius - uboCS.sphereInnerRadius) * 0.1;
        float fogFade = clamp(distBelowInner / fadeRange, 0.0, 1.0);
        fogAmount *= fogFade;
    }
    if (fogAmount > 0.965)
    {
        outFragColor.Store(fragCoord, float4(0.0));
        outBloom.Store(fragCoord, float4(GetSunColor(worldDir, 128) * 1.3, 1.0));
        outAlphaness.Store(fragCoord, float4(0.0));
        outCloudDistance.Store(fragCoord, float4(uboCS.maxDepth, 0.0, 0.0, 0.0));
        return;
    }

    // --- Reprojection ---
    bool doCheckerboard = uboCS.enableCheckerboard != 0;
    bool doTemporalBlend = uboCS.enableTemporalBlend != 0;
    bool doFullReproject = uboCS.enableFullReproject != 0;
    // Checkerboard/full reproject path can early-return without raymarching.
    // Temporal blend is handled later after raymarching using hit-position reprojection.
    bool isCheckerRender = true;
    float4 reprojColor = float4(0.0);
    float reprojDepthVal = uboCS.maxDepth;
    bool reprojValid = false;

    if (doCheckerboard || doFullReproject)
    {
        float2 currentUV = float2(fragCoord.xy) / uboCS.resolution.xy;

        // Checkerboard: alternate which pixels are rendered vs reprojected each frame
        if (doCheckerboard)
        {
            isCheckerRender = ((fragCoord.x + fragCoord.y) & 1) == int(uboCS.frameCounter & 1);
        }

        // Depth-based reprojection: solve previous-frame UV using prev depth + matrix reconstruction.
        // Fixed-point iterations map current pixel to previous pixel robustly under camera motion.
        float2 prevUV = currentUV;
        float2 currUVPred = currentUV;
        reprojValid = false;
        for (int iter = 0; iter < 2; ++iter)
        {
            if (prevUV.x <= 0.001 || prevUV.x >= 0.999 || prevUV.y <= 0.001 || prevUV.y >= 0.999)
            {
                reprojValid = false;
                break;
            }

            reprojDepthVal = prevDepthTex.SampleLevel(prevDepthSampler, prevUV, 0).r;
            if (reprojDepthVal >= (uboCS.maxDepth - 1e-6))
            {
                reprojValid = false;
                break;
            }

            float2 prevNDCxy = prevUV * 2.0 - 1.0;
            float prevNDCz = reprojDepthVal * 2.0 - 1.0;
            float4 prevClipPos = float4(prevNDCxy, prevNDCz, 1.0);
            float4 prevWorldPosH = mul(uboCS.prevInverseViewProjectionMatrix, prevClipPos);
            if (abs(prevWorldPosH.w) < 1e-6)
            {
                reprojValid = false;
                break;
            }

            float3 prevWorldPos = prevWorldPosH.xyz / prevWorldPosH.w;
            float4 currClip = mul(uboCS.currentViewProjectionMatrix, float4(prevWorldPos, 1.0));
            if (currClip.w <= 0.0)
            {
                reprojValid = false;
                break;
            }

            currUVPred = currClip.xy / currClip.w * 0.5 + 0.5;
            prevUV += (currentUV - currUVPred);
            reprojValid = true;
        }

        if (reprojValid && prevUV.x > 0.001 && prevUV.x < 0.999 && prevUV.y > 0.001 && prevUV.y < 0.999)
        {
            // Reject if reprojection error is too large (roughly >2 pixels)
            float2 uvErr = abs(currentUV - currUVPred);
            float2 uvMaxErr = 2.0 / uboCS.resolution.xy;
            reprojValid = (uvErr.x <= uvMaxErr.x && uvErr.y <= uvMaxErr.y);
            if (reprojValid)
            {
                reprojColor = prevColorTex.SampleLevel(prevColorSampler, prevUV, 0);
            }
        }

        // Skip pixel: use reprojected previous frame data
        bool reproject1 = doCheckerboard && !isCheckerRender && reprojValid;
        // Full reproject: on odd frames, reproject ALL pixels (no raymarching)
        bool reproject2 = doFullReproject && (uboCS.frameCounter & 1) == 1 && reprojValid;
        if (reproject1 || reproject2)
        {
            float4 bloomColor = float4(GetSunColor(worldDir, 128) * 1.3, 1.0);
            float cloudAlpha = clamp(reprojColor.a, 0.0, 1.0);
            float4 alphaness = float4(max(cloudAlpha, 0.2), 0.0, 0.0, 1.0);
            if (cloudAlpha > 0.1)
            {
                float fogAmt = ComputeFogAmount(startPos, 0.00003);
                float3 cloud = lerp(float3(0.0), bloomColor.rgb, clamp(fogAmt, 0.0, 1.0));
                bloomColor.rgb = bloomColor.rgb * (1.0 - cloudAlpha) + cloud.rgb;
            }
            //outFragColor.Store(fragCoord, float4(1.0, 0.0, 0.0, 1.0));
            outFragColor.Store(fragCoord, reprojColor);
            outBloom.Store(fragCoord, bloomColor);
            outAlphaness.Store(fragCoord, alphaness);
            outCloudDistance.Store(fragCoord, float4(reprojDepthVal, 0.0, 0.0, 0.0));
            return;
        }
    }

    RaymarchToCloudResult raymarchResult = RaymarchToCloud(startPos, endPos, backgroundColor.rgb, uint2(fragCoord));

    raymarchResult.color.rgb = raymarchResult.color.rgb * 1.8 - 0.1 * raymarchResult.color.a;
    raymarchResult.color.rgb = lerp(raymarchResult.color.rgb, backgroundColor.rgb * raymarchResult.color.a, clamp(fogAmount, 0.0, 1.0));

    float sun = clamp(dot(uboCS.lightDirection.xyz, normalize(endPos - startPos)), 0.0, 1.0);
    float3 s = 0.8 * float3(1.0, 0.4, 0.2) * pow(sun, 256.0);
    raymarchResult.color.rgb += s * raymarchResult.color.a;

    float4 bloomColor = float4(GetSunColor(worldDir, 128) * 1.3, 1.0);

    float cloudAlphaness = clamp(raymarchResult.color.a, 0.0, 1.0);
    float4 alphaness = float4(max(cloudAlphaness, 0.2), 0.0, 0.0, 1.0);
    if (cloudAlphaness > 0.1)
    {
        float fogAmt = ComputeFogAmount(startPos, 0.00003);
        float3 cloud = lerp(float3(0.0), bloomColor.rgb, clamp(fogAmt, 0.0, 1.0));
        bloomColor.rgb = bloomColor.rgb * (1.0 - cloudAlphaness) + cloud.rgb;
    }

    // Store cloud-only color + alpha (pre-composite, no background baked in).
    // Background compositing happens in the post-process shader.
    float4 fragColor = float4(raymarchResult.color.rgb, raymarchResult.color.a);

    // Temporal blend: reproject using the actual current cloud hit position.
    // This is more stable than the far-point approximation used for checkerboard fill.
    if (doTemporalBlend && raymarchResult.hit)
    {
        float4 prevClip = mul(uboCS.prevViewProjectionMatrix, float4(raymarchResult.position, 1.0));
        if (prevClip.w > 0.0)
        {
            float2 prevNDC = prevClip.xy / prevClip.w;
            float2 prevUV = prevNDC * 0.5 + 0.5;
            if (prevUV.x > 0.001 && prevUV.x < 0.999 && prevUV.y > 0.001 && prevUV.y < 0.999)
            {
                float4 temporalColor = prevColorTex.SampleLevel(prevColorSampler, prevUV, 0);
                fragColor = lerp(temporalColor, fragColor, uboCS.reprojectionBlend);
            }
        }
    }

    float4 depth = float4(ComputeDepth(uboCS.projectionMatrix, uboCS.viewMatrix, raymarchResult.position), 0.0, 0.0, 0.0);

    outFragColor.Store(fragCoord, fragColor);
    outBloom.Store(fragCoord, bloomColor);
    outAlphaness.Store(fragCoord, alphaness);
    outCloudDistance.Store(fragCoord, depth);
}
