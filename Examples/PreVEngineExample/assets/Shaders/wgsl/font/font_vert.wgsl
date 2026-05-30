struct FontVSParams_std140_0
{
    @align(16) translation_0 : vec4<f32>,
    @align(16) scale_0 : vec4<f32>,
};

@binding(0) @group(0) var<uniform> uboVS_0 : FontVSParams_std140_0;
struct Interpolants_0
{
    @builtin(position) position_0 : vec4<f32>,
    @location(0) textureCoord_0 : vec2<f32>,
};

struct vertexInput_0
{
    @location(0) position_1 : vec2<f32>,
    @location(1) textureCoord_1 : vec2<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> Interpolants_0
{
    var output_0 : Interpolants_0;
    output_0.position_0 = vec4<f32>((_S1.position_1 + uboVS_0.translation_0.xy * vec2<f32>(2.0f, 2.0f)) * uboVS_0.scale_0.xy, 0.0f, 1.0f);
    output_0.textureCoord_0 = _S1.textureCoord_1;
    return output_0;
}

