struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E100_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(100)>,
};

struct AnimBumpShadowParams_std140_0
{
    @align(16) bones_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E100_0,
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) projectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
};

@binding(0) @group(0) var<uniform> ubo_0 : AnimBumpShadowParams_std140_0;
struct vertexOutput_0
{
    @builtin(position) output_0 : vec4<f32>,
};

struct vertexInput_0
{
    @location(0) position_0 : vec3<f32>,
    @location(1) textureCoord_0 : vec2<f32>,
    @location(2) normal_0 : vec3<f32>,
    @location(3) boneIds_0 : vec4<i32>,
    @location(4) weights_0 : vec4<f32>,
    @location(5) tangent_0 : vec3<f32>,
    @location(6) biTangent_0 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> vertexOutput_0
{
    var _S2 : mat4x4<f32> = mat4x4<f32>(ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(0)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(1)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(2)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(3)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(0)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(1)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(2)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(3)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(0)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(1)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(2)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(3)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(0)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(1)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(2)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(0)]].data_0[i32(3)][i32(3)]);
    var _S3 : mat4x4<f32> = mat4x4<f32>(_S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)], _S1.weights_0[i32(0)]);
    var _S4 : mat4x4<f32> = mat4x4<f32>(ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(0)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(1)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(2)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(3)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(0)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(1)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(2)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(3)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(0)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(1)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(2)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(3)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(0)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(1)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(2)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(1)]].data_0[i32(3)][i32(3)]);
    var _S5 : mat4x4<f32> = mat4x4<f32>(_S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)], _S1.weights_0[i32(1)]);
    var _S6 : mat4x4<f32> = mat4x4<f32>(ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(0)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(1)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(2)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(3)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(0)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(1)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(2)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(3)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(0)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(1)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(2)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(3)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(0)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(1)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(2)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(2)]].data_0[i32(3)][i32(3)]);
    var _S7 : mat4x4<f32> = mat4x4<f32>(_S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)], _S1.weights_0[i32(2)]);
    var _S8 : mat4x4<f32> = mat4x4<f32>(ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(0)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(1)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(2)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(3)][i32(0)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(0)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(1)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(2)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(3)][i32(1)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(0)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(1)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(2)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(3)][i32(2)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(0)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(1)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(2)][i32(3)], ubo_0.bones_0.data_1[_S1.boneIds_0[i32(3)]].data_0[i32(3)][i32(3)]);
    var _S9 : mat4x4<f32> = mat4x4<f32>(_S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)], _S1.weights_0[i32(3)]);
    var _S10 : vertexOutput_0 = vertexOutput_0( (((((((((vec4<f32>((((vec4<f32>(_S1.position_0, 1.0f)) * (mat4x4<f32>(_S2[0] * _S3[0], _S2[1] * _S3[1], _S2[2] * _S3[2], _S2[3] * _S3[3]) + mat4x4<f32>(_S4[0] * _S5[0], _S4[1] * _S5[1], _S4[2] * _S5[2], _S4[3] * _S5[3]) + mat4x4<f32>(_S6[0] * _S7[0], _S6[1] * _S7[1], _S6[2] * _S7[2], _S6[3] * _S7[3]) + mat4x4<f32>(_S8[0] * _S9[0], _S8[1] * _S9[1], _S8[2] * _S9[2], _S8[3] * _S9[3])))).xyz, 1.0f)) * (mat4x4<f32>(ubo_0.modelMatrix_0.data_0[i32(0)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(0)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(1)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(2)], ubo_0.modelMatrix_0.data_0[i32(0)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(1)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(2)][i32(3)], ubo_0.modelMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(ubo_0.viewMatrix_0.data_0[i32(0)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(0)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(1)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(2)], ubo_0.viewMatrix_0.data_0[i32(0)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(1)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(2)][i32(3)], ubo_0.viewMatrix_0.data_0[i32(3)][i32(3)]))))) * (mat4x4<f32>(ubo_0.projectionMatrix_0.data_0[i32(0)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(0)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(1)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(2)], ubo_0.projectionMatrix_0.data_0[i32(0)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(1)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(2)][i32(3)], ubo_0.projectionMatrix_0.data_0[i32(3)][i32(3)])))) );
    return _S10;
}

