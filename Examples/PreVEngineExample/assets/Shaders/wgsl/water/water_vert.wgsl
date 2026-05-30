struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(1)>,
};

struct WaterVSParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) cameraPositions_0 : array<vec4<f32>, i32(1)>,
    @align(16) density_0 : f32,
    @align(4) gradient_0 : f32,
};

@binding(0) @group(0) var<uniform> uboVS_0 : WaterVSParams_std140_0;
struct Interpolants_0
{
    @builtin(position) position_0 : vec4<f32>,
    @location(0) clipSpaceCoord_0 : vec4<f32>,
    @location(1) textureCoord_0 : vec2<f32>,
    @location(2) worldPosition_0 : vec3<f32>,
    @location(3) viewPosition_0 : vec3<f32>,
    @location(4) toCameraVector_0 : vec3<f32>,
    @location(5) visibility_0 : f32,
};

struct vertexInput_0
{
    @location(0) position_1 : vec3<f32>,
    @location(1) textureCoord_1 : vec2<f32>,
    @location(2) normal_0 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> Interpolants_0
{
    var worldPosition_1 : vec4<f32> = (((vec4<f32>(_S1.position_1, 1.0f)) * (mat4x4<f32>(uboVS_0.modelMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(3)]))));
    var output_0 : Interpolants_0;
    var _S2 : vec3<f32> = worldPosition_1.xyz;
    output_0.worldPosition_0 = _S2;
    var viewPosition_1 : vec4<f32> = (((worldPosition_1) * (mat4x4<f32>(uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    var _S3 : vec3<f32> = viewPosition_1.xyz;
    output_0.viewPosition_0 = _S3;
    var _S4 : vec4<f32> = (((viewPosition_1) * (mat4x4<f32>(uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    output_0.clipSpaceCoord_0 = _S4;
    output_0.position_0 = _S4;
    output_0.textureCoord_0 = _S1.position_1.xz / vec2<f32>(2.0f) + vec2<f32>(0.5f);
    output_0.toCameraVector_0 = uboVS_0.cameraPositions_0[i32(0)].xyz - _S2;
    output_0.visibility_0 = clamp(exp(- pow(length(_S3) * uboVS_0.density_0, uboVS_0.gradient_0)), 0.0f, 1.0f);
    return output_0;
}

