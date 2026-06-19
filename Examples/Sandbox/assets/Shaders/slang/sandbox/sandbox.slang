// Lit sandbox shader. Multiview-aware (1 view in mono, 2 for stereo XR).
//
// Matrices are passed separately rather than pre-combined into an MVP: the vertex stage builds
// world -> view -> clip itself and forwards the world-space position/normal. That gives any
// prototype shader access to world and view space without changing the C++ uniform layout.

#ifndef MAX_VIEW_COUNT
#define MAX_VIEW_COUNT 1
#endif

struct SandboxParams
{
    float4x4 modelMatrix;                        // object -> world
    float4x4 normalMatrix;                       // object -> world for normals (model, rotation-only here)
    float4x4 viewMatrices[MAX_VIEW_COUNT];       // world -> eye, per view
    float4x4 projectionMatrices[MAX_VIEW_COUNT]; // eye -> clip, per view
    float4 lightDirection;                       // world-space direction TO the light (xyz)
    float4 color;                                // base color (rgb)
};

[[vk::binding(0)]] ConstantBuffer<SandboxParams> ubo;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 normal : NORMAL;
    [[vk::location(1)]] float3 worldPosition : POSITION; // handy for world-space effects
};

[shader("vertex")]
VertexOutput vertexMain(VertexInput input
#ifdef ENABLE_XR
    , uint viewIndex : SV_ViewID // multiview: the layer (eye) currently being rendered
#endif
)
{
#ifndef ENABLE_XR
    const uint viewIndex = 0; // mono: a single view
#endif
    VertexOutput output;

    const float4 worldPosition = mul(ubo.modelMatrix, float4(input.position, 1.0));
    const float4 viewPosition = mul(ubo.viewMatrices[viewIndex], worldPosition);
    output.position = mul(ubo.projectionMatrices[viewIndex], viewPosition);

    output.normal = mul(ubo.normalMatrix, float4(input.normal, 0.0)).xyz;
    output.worldPosition = worldPosition.xyz;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.normal);
    float diffuse = max(dot(normal, normalize(ubo.lightDirection.xyz)), 0.0);
    float3 color = ubo.color.rgb * (0.2 + 0.8 * diffuse); // ambient + diffuse
    return float4(color, 1.0);
}
