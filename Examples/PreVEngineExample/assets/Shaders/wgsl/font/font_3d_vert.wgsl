struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(1)>,
};

struct Font3DVSParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) clipPlane_0 : vec4<f32>,
};

@binding(0) @group(0) var<uniform> uboVS_0 : Font3DVSParams_std140_0;
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
    output_0.position_0 = (((((((((vec4<f32>(_S1.position_1.x, - _S1.position_1.y, 0.0f, 1.0f)) * (mat4x4<f32>(uboVS_0.modelMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    output_0.textureCoord_0 = _S1.textureCoord_1;
    return output_0;
}

