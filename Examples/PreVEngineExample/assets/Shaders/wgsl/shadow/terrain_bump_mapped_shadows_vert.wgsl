struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct ShadowParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) projectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
};

@binding(0) @group(0) var<uniform> ubo_0 : ShadowParams_std140_0;
struct vertexOutput_0
{
    @builtin(position) output_0 : vec4<f32>,
};

struct vertexInput_0
{
    @location(0) position_0 : vec3<f32>,
    @location(1) textureCoord_0 : vec2<f32>,
    @location(2) normal_0 : vec3<f32>,
    @location(3) tangent_0 : vec3<f32>,
    @location(4) biTangent_0 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> vertexOutput_0
{
    var _S2 : vertexOutput_0 = vertexOutput_0( (((((((((vec4<f32>(_S1.position_0, 1.0f)) * (mat4x4<f32>(ubo_0.modelMatrix_0.data_0[i32(0)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(ubo_0.viewMatrix_0.data_0[i32(0)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(ubo_0.projectionMatrix_0.data_0[i32(0)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(3)])))) );
    return _S2;
}

