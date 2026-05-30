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

struct TerrainNMVSParams_std140_0
{
    @align(16) modelMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) normalMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) projectionMatrices_0 : _Array_std140_matrixx3Cfloatx2C4x2C4x3E1_0,
    @align(16) cameraPositions_0 : array<vec4<f32>, i32(1)>,
    @align(16) clipPlane_0 : vec4<f32>,
    @align(16) lightning_0 : Lightning_std140_0,
    @align(16) density_0 : f32,
    @align(4) gradient_0 : f32,
};

@binding(0) @group(0) var<uniform> uboVS_0 : TerrainNMVSParams_std140_0;
fn GetVisibility_0( viewPosition_0 : vec3<f32>,  gradient_1 : f32,  density_1 : f32) -> f32
{
    return clamp(exp(- pow(length(viewPosition_0) * density_1, gradient_1)), 0.0f, 1.0f);
}

fn CreateTBNMatrix_0( transform_0 : mat3x3<f32>,  normal_0 : vec3<f32>,  tangent_0 : vec3<f32>,  biTangent_0 : vec3<f32>) -> mat3x3<f32>
{
    return mat3x3<f32>(normalize((((tangent_0) * (transform_0)))), normalize((((biTangent_0) * (transform_0)))), normalize((((normal_0) * (transform_0)))));
}

struct Interpolants_0
{
    @builtin(position) position_1 : vec4<f32>,
    @location(0) textureCoord_0 : vec2<f32>,
    @location(11) normal_1 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
    @location(2) viewPosition_1 : vec3<f32>,
    @location(3) visibility_0 : f32,
    @location(4) toCameraVectorTangentSpace_0 : vec3<f32>,
    @location(5) positionTangentSpace_0 : vec3<f32>,
    @location(6) toLightVectorTangentSpace0_0 : vec3<f32>,
    @location(7) toLightVectorTangentSpace1_0 : vec3<f32>,
    @location(8) toLightVectorTangentSpace2_0 : vec3<f32>,
    @location(9) toLightVectorTangentSpace3_0 : vec3<f32>,
    @location(10) clipDistance_0 : f32,
};

struct vertexInput_0
{
    @location(0) position_2 : vec3<f32>,
    @location(1) textureCoord_1 : vec2<f32>,
    @location(2) normal_2 : vec3<f32>,
    @location(3) tangent_1 : vec3<f32>,
    @location(4) biTangent_1 : vec3<f32>,
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
    output_0.textureCoord_0 = _S1.textureCoord_1;
    output_0.normal_1 = (((vec4<f32>(_S1.normal_2, 0.0f)) * (mat4x4<f32>(uboVS_0.normalMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.normalMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.normalMatrix_0.data_0[i32(3)][i32(3)])))).xyz;
    output_0.visibility_0 = GetVisibility_0(_S3, uboVS_0.gradient_0, uboVS_0.density_0);
    var _S4 : mat4x4<f32> = mat4x4<f32>(uboVS_0.modelMatrix_0.data_0[i32(0)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(0)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(1)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(2)], uboVS_0.modelMatrix_0.data_0[i32(0)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(1)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(2)][i32(3)], uboVS_0.modelMatrix_0.data_0[i32(3)][i32(3)]);
    var TBN_0 : mat3x3<f32> = CreateTBNMatrix_0(mat3x3<f32>(_S4[i32(0)].xyz, _S4[i32(1)].xyz, _S4[i32(2)].xyz), _S1.normal_2, _S1.tangent_1, _S1.biTangent_1);
    output_0.toCameraVectorTangentSpace_0 = (((uboVS_0.cameraPositions_0[i32(0)].xyz) * (TBN_0)));
    output_0.positionTangentSpace_0 = (((_S2) * (TBN_0)));
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
        toLightVectors_0[i_0] = (((uboVS_0.lightning_0.lights_0.data_2[i_0].position_0.xyz) * (TBN_0)));
        i_0 = i_0 + i32(1);
    }
    output_0.toLightVectorTangentSpace0_0 = toLightVectors_0[i32(0)];
    output_0.toLightVectorTangentSpace1_0 = toLightVectors_0[i32(1)];
    output_0.toLightVectorTangentSpace2_0 = toLightVectors_0[i32(2)];
    output_0.toLightVectorTangentSpace3_0 = toLightVectors_0[i32(3)];
    return output_0;
}

