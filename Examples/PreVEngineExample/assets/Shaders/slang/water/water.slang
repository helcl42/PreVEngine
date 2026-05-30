// Water shader with reflection/refraction
import common.utils;
import common.shadows;

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct WaterLight
{
    float4 position;
    float4 color;
};

struct WaterVSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrices[MAX_VIEW_COUNT];
    float4x4 projectionMatrices[MAX_VIEW_COUNT];
    float4 cameraPositions[MAX_VIEW_COUNT];
    float density;
    float gradient;
};

struct WaterFSParams
{
    Shadows shadows;
    float4 fogColor;
    float4 waterColor;
    WaterLight light;
    float4 nearFarClippinPlane;
    float4 viewportSize;
    float moveFactor;
};

[[vk::binding(0)]] ConstantBuffer<WaterVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<WaterFSParams> uboFS;
[[vk::binding(2)]] Texture2DArray depthTexture;
[[vk::binding(3)]] SamplerState depthSampler;
#ifdef ENABLE_XR
[[vk::binding(4)]] Texture2DArray reflectionTexture;
[[vk::binding(5)]] SamplerState reflectionSampler;
[[vk::binding(6)]] Texture2DArray refractionTexture;
[[vk::binding(7)]] SamplerState refractionSampler;
[[vk::binding(8)]] Texture2DArray depthMapTexture;
[[vk::binding(9)]] SamplerState depthMapSampler;
#else
[[vk::binding(4)]] Texture2D reflectionTexture;
[[vk::binding(5)]] SamplerState reflectionSampler;
[[vk::binding(6)]] Texture2D refractionTexture;
[[vk::binding(7)]] SamplerState refractionSampler;
[[vk::binding(8)]] Texture2D depthMapTexture;
[[vk::binding(9)]] SamplerState depthMapSampler;
#endif
[[vk::binding(10)]] Texture2D dudvMapTexture;
[[vk::binding(11)]] SamplerState dudvMapSampler;
[[vk::binding(12)]] Texture2D normalMapTexture;
[[vk::binding(13)]] SamplerState normalMapSampler;

static const float textureTilingFactor = 1.0;
static const float waveStrength = 0.04;
static const float shineDamper = 20.0;
static const float reflectivity = 0.45;
static const float waterReflectivness = 0.7;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct Interpolants
{
    float4 position : SV_Position;
    [[vk::location(0)]] float4 clipSpaceCoord : TEXCOORD0;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD1;
    [[vk::location(2)]] float3 worldPosition : TEXCOORD2;
    [[vk::location(3)]] float3 viewPosition : TEXCOORD3;
    [[vk::location(4)]] float3 toCameraVector : TEXCOORD4;
    [[vk::location(5)]] float visibility : TEXCOORD5;
};

[shader("vertex")]
Interpolants vertexMain(VertexInput input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID
#endif
)
{
#ifndef ENABLE_XR
    const int viewIndex = 0;
#endif
    Interpolants output;

    float4 worldPosition = mul(uboVS.modelMatrix, float4(input.position, 1.0));
    output.worldPosition = worldPosition.xyz;

    float4 viewPosition = mul(uboVS.viewMatrices[viewIndex], worldPosition);
    output.viewPosition = viewPosition.xyz;

    output.clipSpaceCoord = mul(uboVS.projectionMatrices[viewIndex], viewPosition);
    output.position = output.clipSpaceCoord;

    output.textureCoord = float2(input.position.xz / 2.0 + 0.5) * textureTilingFactor;
    output.toCameraVector = uboVS.cameraPositions[viewIndex].xyz - worldPosition.xyz;

    float vertexToCameraDistance = length(viewPosition.xyz);
    output.visibility = clamp(exp(-pow(vertexToCameraDistance * uboVS.density, uboVS.gradient)), 0.0, 1.0);
    return output;
}

[shader("fragment")]
float4 fragmentMain(Interpolants input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID
#endif
) : SV_Target
{
#ifndef ENABLE_XR
    const int viewIndex = 0;
#endif

    float shadow = GetShadow(depthTexture, depthSampler, uboFS.shadows, input.viewPosition, input.worldPosition, 0.005, input.position);
    if (shadow < 0.999)
    {
        shadow = 0.0;
    }

    float2 normalizedDeviceSpaceCoord = input.position.xy / uboFS.viewportSize.xy;

    float2 reflectTexCoord = float2(normalizedDeviceSpaceCoord.x, 1.0 - normalizedDeviceSpaceCoord.y);
    float2 refractTexCoord = float2(normalizedDeviceSpaceCoord.x, normalizedDeviceSpaceCoord.y);

    // Calculate water depth
#ifdef ENABLE_XR
    float depth = depthMapTexture.Sample(depthMapSampler, float3(refractTexCoord, (float)viewIndex)).r;
#else
    float depth = depthMapTexture.Sample(depthMapSampler, refractTexCoord).r;
#endif
    float floorDistance = LinearizeDepth(depth, uboFS.nearFarClippinPlane.x, uboFS.nearFarClippinPlane.y);
    float waterDistance = LinearizeDepth(input.position.z, uboFS.nearFarClippinPlane.x, uboFS.nearFarClippinPlane.y);
    float waterDepth = floorDistance - waterDistance;

    // Distortion
    float2 distortedTexCoords = dudvMapTexture.Sample(dudvMapSampler, float2(input.textureCoord.x + uboFS.moveFactor, input.textureCoord.y)).rg * 0.1;
    distortedTexCoords = input.textureCoord + float2(distortedTexCoords.x, distortedTexCoords.y + uboFS.moveFactor);
    float2 totalDistortion = (dudvMapTexture.Sample(dudvMapSampler, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 12.0, 0.0, 1.0);

    reflectTexCoord += totalDistortion;
    reflectTexCoord = clamp(reflectTexCoord, 0.001, 0.999);
    refractTexCoord += totalDistortion;
    refractTexCoord = clamp(refractTexCoord, 0.001, 0.999);

#ifdef ENABLE_XR
    float4 reflectColor = reflectionTexture.Sample(reflectionSampler, float3(reflectTexCoord, (float)viewIndex));
    float4 refractColor = refractionTexture.Sample(refractionSampler, float3(refractTexCoord, (float)viewIndex));
#else
    float4 reflectColor = reflectionTexture.Sample(reflectionSampler, reflectTexCoord);
    float4 refractColor = refractionTexture.Sample(refractionSampler, refractTexCoord);
#endif
    float4 finalWaterColor = uboFS.waterColor * shadow;

    // Normal
    float4 normalMapColor = normalMapTexture.Sample(normalMapSampler, distortedTexCoords);
    float3 normal = float3(normalMapColor.r * 2.0 - 1.0, normalMapColor.g * 3.0, normalMapColor.b * 2.0 - 1.0);
    normal = normalize(normal);

    float3 viewVector = normalize(input.toCameraVector);
    float refractiveFactor = dot(viewVector, normal);
    refractiveFactor = pow(refractiveFactor, waterReflectivness);
    refractiveFactor = clamp(refractiveFactor, 0.001, 0.999);

    float3 toLightVector = uboFS.light.position.xyz - input.worldPosition.xyz;
    float3 reflectedLight = reflect(normalize(-toLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    float3 specularHighlights = uboFS.light.color.xyz * specular * reflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

    float4 baseResultColor = lerp(reflectColor, refractColor, refractiveFactor);
    baseResultColor = clamp(lerp(baseResultColor, finalWaterColor, 0.2) + float4(specularHighlights, 1.0), 0.0, 1.0);

    float4 outColor = baseResultColor;
    outColor.a = clamp(waterDepth / 5.0, 0.0, 1.0);
    return outColor;
}
