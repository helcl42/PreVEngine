struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(1)>,
};

struct SandboxParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) normalMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) lightDirection_0 : vec4<f32>,
    @align(16) color_0 : vec4<f32>,
};

@binding(0) @group(0) var<uniform> ubo_0 : SandboxParams_std140_0;
struct VertexOutput_0
{
    @builtin(position) position_0 : vec4<f32>,
    @location(0) normal_0 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
};

struct vertexInput_0
{
    @location(0) position_1 : vec3<f32>,
    @location(1) normal_1 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> VertexOutput_0
{
    var worldPosition_1 : vec4<f32> = (((vec4<f32>(_S1.position_1, 1.0f)) * (mat4x4<f32>(ubo_0.modelMatrix_0.data_0[i32(0)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(3)]))));
    var output_0 : VertexOutput_0;
    output_0.position_0 = ((((((worldPosition_1) * (mat4x4<f32>(ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(0)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(0)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(0)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(0)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(1)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(1)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(1)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(1)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(2)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(2)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(2)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(2)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(3)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(3)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(3)], ubo_0.viewMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(0)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(0)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(0)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(0)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(1)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(1)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(1)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(1)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(2)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(2)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(2)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(2)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(0)][i32(3)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(1)][i32(3)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(2)][i32(3)], ubo_0.projectionMatrices_0.data_1[u32(0)].data_0[i32(3)][i32(3)]))));
    output_0.normal_0 = (((vec4<f32>(_S1.normal_1, 0.0f)) * (mat4x4<f32>(ubo_0.normalMatrix_0.data_0[i32(0)][i32(0)], ubo_0.normalMatrix_0.data_0[i32(1)][i32(0)], ubo_0.normalMatrix_0.data_0[i32(2)][i32(0)], ubo_0.normalMatrix_0.data_0[i32(3)][i32(0)], ubo_0.normalMatrix_0.data_0[i32(0)][i32(1)], ubo_0.normalMatrix_0.data_0[i32(1)][i32(1)], ubo_0.normalMatrix_0.data_0[i32(2)][i32(1)], ubo_0.normalMatrix_0.data_0[i32(3)][i32(1)], ubo_0.normalMatrix_0.data_0[i32(0)][i32(2)], ubo_0.normalMatrix_0.data_0[i32(1)][i32(2)], ubo_0.normalMatrix_0.data_0[i32(2)][i32(2)], ubo_0.normalMatrix_0.data_0[i32(3)][i32(2)], ubo_0.normalMatrix_0.data_0[i32(0)][i32(3)], ubo_0.normalMatrix_0.data_0[i32(1)][i32(3)], ubo_0.normalMatrix_0.data_0[i32(2)][i32(3)], ubo_0.normalMatrix_0.data_0[i32(3)][i32(3)])))).xyz;
    output_0.worldPosition_0 = worldPosition_1.xyz;
    return output_0;
}

