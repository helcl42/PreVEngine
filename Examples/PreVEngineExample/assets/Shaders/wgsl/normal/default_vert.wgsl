struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0
{
    @align(16) data_1 : array<_MatrixStorage_float4x4_ColMajorstd140_0, i32(1)>,
};

struct Light_std140_0
{
    @align(16) position_0 : vec4<f32>,
    @align(16) color_0 : vec4<f32>,
    @align(16) attenuation_0 : vec4<f32>,
};

struct _Array_std140_Light4_0
{
    @align(16) data_2 : array<Light_std140_0, i32(4)>,
};

struct Lightning_std140_0
{
    @align(16) lights_0 : _Array_std140_Light4_0,
    @align(16) realCountOfLights_0 : u32,
    @align(4) ambientFactor_0 : f32,
};

struct DefaultVSParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) normalMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) cameraPositions_0 : array<vec4<f32>, i32(1)>,
    @align(16) clipPlane_0 : vec4<f32>,
    @align(16) lightning_0 : Lightning_std140_0,
    @align(16) textureOffset_0 : vec4<f32>,
    @align(16) textureNumberOfRows_0 : u32,
    @align(4) density_0 : f32,
    @align(8) gradient_0 : f32,
};

@binding(0) @group(0) var<uniform> uboVS_0 : DefaultVSParams_std140_0;
fn GetVisibility_0( viewPosition_0 : vec3<f32>,  gradient_1 : f32,  density_1 : f32) -> f32
{
    return clamp(exp(- pow(length(viewPosition_0) * density_1, gradient_1)), 0.0f, 1.0f);
}

struct Interpolants_0
{
    @builtin(position) position_1 : vec4<f32>,
    @location(0) textureCoord_0 : vec2<f32>,
    @location(10) normal_0 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
    @location(2) viewPosition_1 : vec3<f32>,
    @location(3) toCameraVector_0 : vec3<f32>,
    @location(4) visibility_0 : f32,
    @location(5) toLightVector0_0 : vec3<f32>,
    @location(6) toLightVector1_0 : vec3<f32>,
    @location(7) toLightVector2_0 : vec3<f32>,
    @location(8) toLightVector3_0 : vec3<f32>,
    @location(9) clipDistance_0 : f32,
};

struct vertexInput_0
{
    @location(0) position_2 : vec3<f32>,
    @location(1) textureCoord_1 : vec2<f32>,
    @location(2) normal_1 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> Interpolants_0
{
    var worldPosition_1 : vec4<f32> = (((vec4<f32>(_S1.position_2, 1.0f)) * (mat4x4<f32>(uboVS_0.modelMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(3)]))));
    var output_0 : Interpolants_0;
    var _S2 : vec3<f32> = worldPosition_1.xyz;
    output_0.worldPosition_0 = _S2;
    output_0.clipDistance_0 = dot(worldPosition_1, uboVS_0.clipPlane_0);
    var viewPosition_2 : vec4<f32> = (((worldPosition_1) * (mat4x4<f32>(uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.viewMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    var _S3 : vec3<f32> = viewPosition_2.xyz;
    output_0.viewPosition_1 = _S3;
    output_0.position_1 = (((viewPosition_2) * (mat4x4<f32>(uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(0)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(1)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(2)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(0)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(1)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(2)][i32(3)], uboVS_0.projectionMatrices_0.data_1[i32(0)].data_0[i32(3)][i32(3)]))));
    output_0.textureCoord_0 = _S1.textureCoord_1 / vec2<f32>(f32(uboVS_0.textureNumberOfRows_0)) + uboVS_0.textureOffset_0.xy;
    output_0.normal_0 = (((vec4<f32>(_S1.normal_1, 0.0f)) * (mat4x4<f32>(uboVS_0.normalMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(3)])))).xyz;
    var toLightVectors_0 : array<vec3<f32>, i32(4)>;
    var i_0 : i32 = i32(0);
    for(;;)
    {
        if(i_0 < i32(uboVS_0.lightning_0.realCountOfLights_0))
        {
        }
        else
        {
            break;
        }
        toLightVectors_0[i_0] = uboVS_0.lightning_0.lights_0.data_2[i_0].position_0.xyz - _S2;
        i_0 = i_0 + i32(1);
    }
    output_0.toCameraVector_0 = uboVS_0.cameraPositions_0[i32(0)].xyz - _S2;
    output_0.visibility_0 = GetVisibility_0(_S3, uboVS_0.gradient_0, uboVS_0.density_0);
    output_0.toLightVector0_0 = toLightVectors_0[i32(0)];
    output_0.toLightVector1_0 = toLightVectors_0[i32(1)];
    output_0.toLightVector2_0 = toLightVectors_0[i32(2)];
    output_0.toLightVector3_0 = toLightVectors_0[i32(3)];
    return output_0;
}

