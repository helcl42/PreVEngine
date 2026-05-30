struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(1)>,
};

struct ParticleVSParams_std140_0
{
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) textureNumberOfRows_0 : u32,
};

@binding(0) @group(0) var<uniform> uboVS_0 : ParticleVSParams_std140_0;
fn MakeRotation_0( angle_0 : f32) -> mat2x2<f32>
{
    var s_0 : f32 = sin(angle_0);
    var c_0 : f32 = cos(angle_0);
    return mat2x2<f32>(c_0, s_0, - s_0, c_0);
}

struct Interpolants_0
{
    @builtin(position) position_0 : vec4<f32>,
    @location(0) currentStageTextureCoord_0 : vec2<f32>,
    @location(1) nextStageTextureCoord_0 : vec2<f32>,
    @location(2) currentNextStageBlendFactor_0 : f32,
};

struct vertexInput_0
{
    @location(0) position_1 : vec3<f32>,
    @location(7) textureCoord_0 : vec2<f32>,
    @location(8) normal_0 : vec3<f32>,
    @location(1) instancePosition_0 : vec3<f32>,
    @location(2) scale_0 : vec2<f32>,
    @location(3) rotation_0 : f32,
    @location(4) textureOffsetsCurrent_0 : vec2<f32>,
    @location(5) textureOffsetNext_0 : vec2<f32>,
    @location(6) blendFactor_0 : f32,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> Interpolants_0
{
    var _S2 : mat4x4<f32> = mat4x4<f32>(uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]);
    var localPosition_0 : vec2<f32> = (((vec2<f32>(_S1.position_1.x * _S1.scale_0.x, _S1.position_1.y * _S1.scale_0.y)) * (MakeRotation_0(_S1.rotation_0))));
    var output_0 : Interpolants_0;
    output_0.position_0 = ((((((vec4<f32>(_S1.instancePosition_0 + _S2[i32(0)].xyz * vec3<f32>(- localPosition_0.x) + _S2[i32(1)].xyz * vec3<f32>(localPosition_0.y), 1.0f)) * (_S2)))) * (mat4x4<f32>(uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    var textureCoordBase_0 : vec2<f32> = _S1.textureCoord_0 / vec2<f32>(f32(uboVS_0.textureNumberOfRows_0));
    output_0.currentStageTextureCoord_0 = textureCoordBase_0 + _S1.textureOffsetsCurrent_0;
    output_0.nextStageTextureCoord_0 = textureCoordBase_0 + _S1.textureOffsetNext_0;
    output_0.currentNextStageBlendFactor_0 = _S1.blendFactor_0;
    return output_0;
}

