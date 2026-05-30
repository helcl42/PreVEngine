struct SunOcclusionVSParams_std140_0
{
    @align(16) translations_0 : array<vec4<f32>, i32(1)>,
    @align(16) scale_0 : vec4<f32>,
};

@binding(0) @group(0) var<uniform> uboVS_0 : SunOcclusionVSParams_std140_0;
struct VertexOutput_0
{
    @builtin(position) position_0 : vec4<f32>,
};

struct vertexInput_0
{
    @location(0) position_1 : vec3<f32>,
    @location(1) textureCoord_0 : vec2<f32>,
    @location(2) normal_0 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> VertexOutput_0
{
    var translation_0 : vec3<f32> = uboVS_0.translations_0[i32(0)].xyz;
    var output_0 : VertexOutput_0;
    output_0.position_0 = vec4<f32>(_S1.position_1.xy * uboVS_0.scale_0.xy + translation_0.xy, translation_0.z, 1.0f);
    return output_0;
}

