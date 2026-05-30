// Raycast debug shader (vertex + geometry + fragment)
// NOTE: Geometry shaders are NOT supported in WGSL target.
// This shader will only compile to SPIR-V.

struct RaycastDebugVSParams
{
    float3 color;
};

struct RaycastDebugGSParams
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

[[vk::binding(0)]] ConstantBuffer<RaycastDebugVSParams> uboVS;
[[vk::binding(1)]] ConstantBuffer<RaycastDebugGSParams> uboGS;

static const float SIZE = 0.2;
static const float3 LIGHT_DIRECTION = normalize(float3(0.4, -1.0, 0.8));
static const float AMBIENT_LIGHT = 0.3;

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 textureCoord : TEXCOORD0;
    [[vk::location(2)]] float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 color : COLOR;
};

[shader("vertex")]
VSOutput vertexMain(VertexInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0);
    output.color = uboVS.color;
    return output;
}

struct GSOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 finalColor : COLOR;
};

float3 CalculateLightColor(float3 normal, float3 inputColor)
{
    float brightness = max(dot(-LIGHT_DIRECTION, normal), AMBIENT_LIGHT);
    return inputColor * brightness;
}

[shader("geometry")]
[maxvertexcount(24)]
void geometryMain(point VSOutput input[1], inout TriangleStream<GSOutput> outputStream)
{
    float3 inputColor = input[0].color;
    float4 basePos = input[0].position;

    GSOutput output;

    // Front face
    float3 faceColor = CalculateLightColor(float3(0.0, 0.0, 1.0), inputColor);
    float3 offsets[4] = { float3(-1.0, 1.0, 1.0), float3(-1.0, -1.0, 1.0), float3(1.0, 1.0, 1.0), float3(1.0, -1.0, 1.0) };
    for (int i = 0; i < 4; i++)
    {
        float4 worldPosition = basePos + float4(offsets[i] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();

    // Right face
    faceColor = CalculateLightColor(float3(1.0, 0.0, 0.0), inputColor);
    float3 rightOffsets[4] = { float3(1.0, 1.0, 1.0), float3(1.0, -1.0, 1.0), float3(1.0, 1.0, -1.0), float3(1.0, -1.0, -1.0) };
    for (int j = 0; j < 4; j++)
    {
        float4 worldPosition = basePos + float4(rightOffsets[j] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();

    // Back face
    faceColor = CalculateLightColor(float3(0.0, 0.0, -1.0), inputColor);
    float3 backOffsets[4] = { float3(1.0, 1.0, -1.0), float3(1.0, -1.0, -1.0), float3(-1.0, 1.0, -1.0), float3(-1.0, -1.0, -1.0) };
    for (int k = 0; k < 4; k++)
    {
        float4 worldPosition = basePos + float4(backOffsets[k] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();

    // Left face
    faceColor = CalculateLightColor(float3(-1.0, 0.0, 0.0), inputColor);
    float3 leftOffsets[4] = { float3(-1.0, 1.0, -1.0), float3(-1.0, -1.0, -1.0), float3(-1.0, 1.0, 1.0), float3(-1.0, -1.0, 1.0) };
    for (int l = 0; l < 4; l++)
    {
        float4 worldPosition = basePos + float4(leftOffsets[l] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();

    // Top face
    faceColor = CalculateLightColor(float3(0.0, 1.0, 0.0), inputColor);
    float3 topOffsets[4] = { float3(1.0, 1.0, 1.0), float3(1.0, 1.0, -1.0), float3(-1.0, 1.0, 1.0), float3(-1.0, 1.0, -1.0) };
    for (int m = 0; m < 4; m++)
    {
        float4 worldPosition = basePos + float4(topOffsets[m] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();

    // Bottom face
    faceColor = CalculateLightColor(float3(0.0, -1.0, 0.0), inputColor);
    float3 bottomOffsets[4] = { float3(-1.0, -1.0, 1.0), float3(-1.0, -1.0, -1.0), float3(1.0, -1.0, 1.0), float3(1.0, -1.0, -1.0) };
    for (int n = 0; n < 4; n++)
    {
        float4 worldPosition = basePos + float4(bottomOffsets[n] * SIZE, 0.0);
        output.position = mul(uboGS.projectionMatrix, mul(uboGS.viewMatrix, mul(uboGS.modelMatrix, worldPosition)));
        output.finalColor = faceColor;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}

[shader("fragment")]
float4 fragmentMain(GSOutput input) : SV_Target
{
    return float4(input.finalColor, 1.0);
}
