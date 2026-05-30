struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct SelectionVSParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) projectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
};

@binding(0) @group(0) var<uniform> uboVS_0 : SelectionVSParams_std140_0;
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
    var output_0 : VertexOutput_0;
    output_0.position_0 = (((((((((vec4<f32>(_S1.position_1, 1.0f)) * (mat4x4<f32>(uboVS_0.modelMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(uboVS_0.viewMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.viewMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.viewMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.viewMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.viewMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.viewMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.viewMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.viewMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.viewMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.viewMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.viewMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.viewMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.viewMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.viewMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.viewMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.viewMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(uboVS_0.projectionMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.projectionMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.projectionMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.projectionMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.projectionMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.projectionMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.projectionMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.projectionMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.projectionMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.projectionMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.projectionMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.projectionMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.projectionMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.projectionMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.projectionMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.projectionMatrix_0.data_0[i32(3)][i32(3)]))));
    return output_0;
}

