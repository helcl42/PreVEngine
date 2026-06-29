struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct ShadowsCascade_std140_0
{
    @align(16) viewProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) split_0 : vec4<f32>,
};

struct _Array_std140_ShadowsCascade4_0
{
    @align(16) data_1 : array<ShadowsCascade_std140_0, i32(4)>,
};

struct Shadows_std140_0
{
    @align(16) cascades_0 : _Array_std140_ShadowsCascade4_0,
    @align(16) enabled_0 : u32,
    @align(4) useReverseDepth_0 : u32,
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

struct Material_std140_0
{
    @align(16) color_1 : vec4<f32>,
    @align(16) shineDamper_0 : f32,
    @align(4) reflectivity_0 : f32,
};

struct _Array_std140_Material4_0
{
    @align(16) data_3 : array<Material_std140_0, i32(4)>,
};

struct TerrainCSMFSParams_std140_0
{
    @align(16) shadows_0 : Shadows_std140_0,
    @align(16) lightning_0 : Lightning_std140_0,
    @align(16) material_0 : _Array_std140_Material4_0,
    @align(16) fogColor_0 : vec4<f32>,
    @align(16) selectedColor_0 : vec4<f32>,
    @align(16) selected_0 : u32,
    @align(4) castedByShadows_0 : u32,
    @align(8) minHeight_0 : f32,
    @align(4) maxHeight_0 : f32,
    @align(16) heightSteps_0 : array<vec4<f32>, i32(4)>,
    @align(16) heightScale_0 : array<vec4<f32>, i32(4)>,
    @align(16) heightTransitionRange_0 : f32,
    @align(4) numLayers_0 : u32,
    @align(8) hasNormalMap_0 : u32,
    @align(4) hasConeMap_0 : u32,
};

@binding(1) @group(0) var<uniform> uboFS_0 : TerrainCSMFSParams_std140_0;
@binding(6) @group(0) var heightTextures_0 : texture_2d_array<f32>;

@binding(7) @group(0) var heightSampler_0 : sampler;

@binding(4) @group(0) var normalTextures_0 : texture_2d_array<f32>;

@binding(5) @group(0) var normalSampler_0 : sampler;

@binding(2) @group(0) var colorTextures_0 : texture_2d_array<f32>;

@binding(3) @group(0) var colorSampler_0 : sampler;

@binding(8) @group(0) var depthTexture_0 : texture_2d_array<f32>;

@binding(9) @group(0) var depthSampler_0 : sampler;

fn GetRayDirection_0( viewDirection_0 : vec3<f32>,  heightScale_1 : f32) -> vec3<f32>
{
    var _S1 : vec3<f32> = normalize(viewDirection_0);
    var v_0 : vec3<f32> = _S1;
    v_0[i32(2)] = abs(_S1.z);
    var db_0 : f32 = 1.0f - v_0.z;
    var db_1 : f32 = db_0 * db_0;
    var db_2 : f32 = db_1 * db_1;
    var _S2 : vec2<f32> = v_0.xy * vec2<f32>((1.0f - db_2 * db_2));
    v_0.x = _S2.x;
    v_0.y = _S2.y;
    var _S3 : vec2<f32> = v_0.xy * vec2<f32>(heightScale_1);
    v_0.x = _S3.x;
    v_0.y = _S3.y;
    return v_0;
}

fn GetInverseHeight_0( height_0 : f32) -> f32
{
    return 1.0f - height_0;
}

fn RelaxedConeStepMapping_0( coneMapTextures_0 : texture_2d_array<f32>,  coneMapSampler_0 : sampler,  layer_0 : u32,  heightScale_2 : f32,  numLayers_1 : u32,  uv_0 : vec2<f32>,  ddxUV_0 : vec2<f32>,  ddyUV_0 : vec2<f32>,  texDir3D_0 : vec3<f32>) -> vec2<f32>
{
    var rayPos_0 : vec3<f32> = vec3<f32>(uv_0, 0.0f);
    var rayDir_0 : vec3<f32> = GetRayDirection_0(texDir3D_0, heightScale_2);
    var rayDir_1 : vec3<f32> = rayDir_0 / vec3<f32>(rayDir_0.z);
    var _S4 : f32 = length(rayDir_1.xy);
    var i_0 : u32 = u32(0);
    var pos_0 : vec3<f32> = rayPos_0;
    for(;;)
    {
        if(i_0 < numLayers_1)
        {
        }
        else
        {
            break;
        }
        var _S5 : vec3<f32> = vec3<f32>(pos_0.xy, f32(layer_0));
        var heightAndCone_0 : vec2<f32> = clamp((textureSampleGrad((coneMapTextures_0), (coneMapSampler_0), ((_S5)).xy, i32(((_S5)).z), (ddxUV_0), (ddyUV_0))).xy, vec2<f32>(0.0f), vec2<f32>(1.0f));
        var _S6 : f32 = heightAndCone_0.y;
        var coneRatio_0 : f32 = _S6 * _S6;
        var pos_1 : vec3<f32> = pos_0 + rayDir_1 * vec3<f32>((coneRatio_0 * (GetInverseHeight_0(heightAndCone_0.x) - pos_0.z) / (_S4 + coneRatio_0)));
        i_0 = i_0 + u32(1);
        pos_0 = pos_1;
    }
    var _S7 : vec3<f32> = vec3<f32>(0.5f);
    var bsRange_0 : vec3<f32> = _S7 * rayDir_1 * vec3<f32>(pos_0.z);
    var _S8 : vec3<f32> = rayPos_0 + bsRange_0;
    var j_0 : u32 = u32(0);
    var bsPosition_0 : vec3<f32> = _S8;
    var bsRange_1 : vec3<f32> = bsRange_0;
    for(;;)
    {
        if(j_0 < u32(6))
        {
        }
        else
        {
            break;
        }
        var _S9 : vec3<f32> = vec3<f32>(bsPosition_0.xy, f32(layer_0));
        var bsRange_2 : vec3<f32> = bsRange_1 * _S7;
        if((bsPosition_0.z) < (GetInverseHeight_0(clamp((textureSampleGrad((coneMapTextures_0), (coneMapSampler_0), ((_S9)).xy, i32(((_S9)).z), (ddxUV_0), (ddyUV_0))).xy, vec2<f32>(0.0f), vec2<f32>(1.0f)).x)))
        {
            bsPosition_0 = bsPosition_0 + bsRange_2;
        }
        else
        {
            bsPosition_0 = bsPosition_0 - bsRange_2;
        }
        j_0 = j_0 + u32(1);
        bsRange_1 = bsRange_2;
    }
    return bsPosition_0.xy;
}

fn sampleNormalMap_0( idx_0 : u32,  uv_1 : vec2<f32>,  ddxUV_1 : vec2<f32>,  ddyUV_1 : vec2<f32>) -> vec3<f32>
{
    var _S10 : vec3<f32> = vec3<f32>(uv_1, f32(idx_0));
    return normalize(vec3<f32>(2.0f) * (textureSampleGrad((normalTextures_0), (normalSampler_0), ((_S10)).xy, i32(((_S10)).z), (ddxUV_1), (ddyUV_1))).xyz - vec3<f32>(1.0f));
}

fn sampleColorTextureGrad_0( idx_1 : u32,  uv_2 : vec2<f32>,  ddxUV_2 : vec2<f32>,  ddyUV_2 : vec2<f32>) -> vec4<f32>
{
    var _S11 : vec3<f32> = vec3<f32>(uv_2, f32(idx_1));
    return (textureSampleGrad((colorTextures_0), (colorSampler_0), ((_S11)).xy, i32(((_S11)).z), (ddxUV_2), (ddyUV_2)));
}

fn GetShadowRawInternal_0( depthTexture_1 : texture_2d_array<f32>,  depthSampler_1 : sampler,  shadowCoord_0 : vec4<f32>,  shadowCoordOffset_0 : vec2<f32>,  cascadeIndex_0 : u32,  depthBias_0 : f32,  useReverseDepth_1 : u32) -> f32
{
    var _S12 : f32 = shadowCoord_0.z;
    var _S13 : bool;
    if(_S12 >= 0.0f)
    {
        _S13 = _S12 <= 1.0f;
    }
    else
    {
        _S13 = false;
    }
    var shadow_0 : f32;
    if(_S13)
    {
        var _S14 : vec3<f32> = vec3<f32>(shadowCoord_0.xy + shadowCoordOffset_0, f32(cascadeIndex_0));
        var depth_0 : f32 = (textureSampleLevel((depthTexture_1), (depthSampler_1), ((_S14)).xy, i32(((_S14)).z), (0.0f))).x;
        if(useReverseDepth_1 != u32(0))
        {
            if(depth_0 > (_S12 + depthBias_0))
            {
                shadow_0 = 0.20000000298023224f;
            }
            else
            {
                shadow_0 = 1.0f;
            }
        }
        else
        {
            if(depth_0 < (_S12 - depthBias_0))
            {
                shadow_0 = 0.20000000298023224f;
            }
            else
            {
                shadow_0 = 1.0f;
            }
        }
    }
    else
    {
        shadow_0 = 1.0f;
    }
    return shadow_0;
}

fn GetShadowPCFInternal_0( depthTexture_2 : texture_2d_array<f32>,  depthSampler_2 : sampler,  shadowCoord_1 : vec4<f32>,  cascadeIndex_1 : u32,  depthBias_1 : f32,  useReverseDepth_2 : u32,  fragCoord_0 : vec4<f32>) -> f32
{
    var width_0 : u32;
    var height_1 : u32;
    var elements_0 : u32;
    {var dim = textureDimensions((depthTexture_2));((width_0)) = dim.x;((height_1)) = dim.y;((elements_0)) = textureNumLayers((depthTexture_2));};
    var texelSize_0 : vec2<f32> = vec2<f32>(1.0f) / vec2<f32>(f32(width_0), f32(height_1));
    var _S15 : vec2<f32> = fract(fragCoord_0.xy * vec2<f32>(0.5f)) + vec2<f32>(0.25f);
    var offset_0 : vec2<f32> = _S15;
    if((_S15.y) > 1.10000002384185791f)
    {
        offset_0[i32(1)] = 0.0f;
    }
    return (GetShadowRawInternal_0(depthTexture_2, depthSampler_2, shadowCoord_1, texelSize_0 * (offset_0 + vec2<f32>(-1.5f, 0.5f)), cascadeIndex_1, depthBias_1, useReverseDepth_2) + GetShadowRawInternal_0(depthTexture_2, depthSampler_2, shadowCoord_1, texelSize_0 * (offset_0 + vec2<f32>(0.5f, 0.5f)), cascadeIndex_1, depthBias_1, useReverseDepth_2) + GetShadowRawInternal_0(depthTexture_2, depthSampler_2, shadowCoord_1, texelSize_0 * (offset_0 + vec2<f32>(-1.5f, -1.5f)), cascadeIndex_1, depthBias_1, useReverseDepth_2) + GetShadowRawInternal_0(depthTexture_2, depthSampler_2, shadowCoord_1, texelSize_0 * (offset_0 + vec2<f32>(0.5f, -1.5f)), cascadeIndex_1, depthBias_1, useReverseDepth_2)) * 0.25f;
}

fn GetShadowSingle_0( depthTexture_3 : texture_2d_array<f32>,  depthSampler_3 : sampler,  shadowCoord_2 : vec4<f32>,  cascadeIndex_2 : u32,  depthBias_2 : f32,  useReverseDepth_3 : u32,  fragCoord_1 : vec4<f32>) -> f32
{
    return GetShadowPCFInternal_0(depthTexture_3, depthSampler_3, shadowCoord_2, cascadeIndex_2, depthBias_2, useReverseDepth_3, fragCoord_1);
}

fn GetAttenuationFactor_0( attenuation_1 : vec3<f32>,  toLightVector_0 : vec3<f32>) -> f32
{
    var toLightDistance_0 : f32 = length(toLightVector_0);
    return attenuation_1.x + attenuation_1.y * toLightDistance_0 + attenuation_1.z * toLightDistance_0 * toLightDistance_0;
}

fn GetDiffuseColor_0( normal_0 : vec3<f32>,  toLightVector_1 : vec3<f32>,  lightColor_0 : vec3<f32>,  attenuationFactor_0 : f32) -> vec3<f32>
{
    return vec3<f32>(max(dot(normal_0, toLightVector_1), 0.0f)) * lightColor_0 / vec3<f32>(attenuationFactor_0);
}

fn GetSpecularColor_0( normal_1 : vec3<f32>,  toLightVector_2 : vec3<f32>,  toCameraVector_0 : vec3<f32>,  lightColor_1 : vec3<f32>,  attenuationFactor_1 : f32,  shineDamper_1 : f32,  reflectivity_1 : f32) -> vec3<f32>
{
    return vec3<f32>((pow(max(dot(normal_1, normalize(toLightVector_2 + toCameraVector_0)), 0.0f), shineDamper_1 * 2.40000009536743164f) * reflectivity_1)) * lightColor_1 / vec3<f32>(attenuationFactor_1);
}

struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) textureCoord_0 : vec2<f32>,
    @location(11) normal_2 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
    @location(2) viewPosition_0 : vec3<f32>,
    @location(3) visibility_0 : f32,
    @location(4) toCameraVectorTangentSpace_0 : vec3<f32>,
    @location(5) positionTangentSpace_0 : vec3<f32>,
    @location(6) toLightVectorTangentSpace0_0 : vec3<f32>,
    @location(7) toLightVectorTangentSpace1_0 : vec3<f32>,
    @location(8) toLightVectorTangentSpace2_0 : vec3<f32>,
    @location(9) toLightVectorTangentSpace3_0 : vec3<f32>,
    @location(10) clipDistance_0 : f32,
};

fn GetShadow_0( _S16 : texture_2d_array<f32>,  _S17 : sampler,  _S18 : vec3<f32>,  _S19 : vec3<f32>,  _S20 : f32,  _S21 : vec4<f32>) -> f32
{
    var shadow_1 : f32;
    if((uboFS_0.shadows_0.enabled_0) != u32(0))
    {
        var cascadeIndex_3 : u32 = u32(0);
        var i_1 : u32 = u32(0);
        shadow_1 = _S20;
        for(;;)
        {
            if(i_1 < u32(3))
            {
            }
            else
            {
                break;
            }
            var cascadeIndex_4 : u32;
            var bias_0 : f32;
            if((uboFS_0.shadows_0.useReverseDepth_0) != u32(0))
            {
                if((_S18.z) > (uboFS_0.shadows_0.cascades_0.data_1[i_1].split_0.x))
                {
                    var bias_1 : f32 = shadow_1 / 1.5f;
                    cascadeIndex_4 = i_1 + u32(1);
                    bias_0 = bias_1;
                }
                else
                {
                    cascadeIndex_4 = cascadeIndex_3;
                    bias_0 = shadow_1;
                }
            }
            else
            {
                if((_S18.z) < (uboFS_0.shadows_0.cascades_0.data_1[i_1].split_0.x))
                {
                    var bias_2 : f32 = shadow_1 / 1.5f;
                    cascadeIndex_4 = i_1 + u32(1);
                    bias_0 = bias_2;
                }
                else
                {
                    cascadeIndex_4 = cascadeIndex_3;
                    bias_0 = shadow_1;
                }
            }
            var _S22 : u32 = i_1 + u32(1);
            cascadeIndex_3 = cascadeIndex_4;
            i_1 = _S22;
            shadow_1 = bias_0;
        }
        var shadowCoord_3 : vec4<f32> = (((vec4<f32>(_S19, 1.0f)) * (mat4x4<f32>(uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(3)]))));
        shadow_1 = GetShadowSingle_0(_S16, _S17, shadowCoord_3 / vec4<f32>(shadowCoord_3.w), cascadeIndex_3, shadow_1, uboFS_0.shadows_0.useReverseDepth_0, _S21);
    }
    else
    {
        shadow_1 = 1.0f;
    }
    return shadow_1;
}

@fragment
fn fragmentMain( _S23 : pixelInput_0, @builtin(position) position_1 : vec4<f32>) -> pixelOutput_0
{
    var totalDiffuse_0 : vec3<f32>;
    var totalSpecular_0 : vec3<f32>;
    var _S24 : array<vec3<f32>, i32(4)> = array<vec3<f32>, i32(4)>( _S23.toLightVectorTangentSpace0_0, _S23.toLightVectorTangentSpace1_0, _S23.toLightVectorTangentSpace2_0, _S23.toLightVectorTangentSpace3_0 );
    if((_S23.clipDistance_0) < 0.0f)
    {
        discard;
    }
    var _S25 : vec3<f32> = normalize(_S23.positionTangentSpace_0 - _S23.toCameraVectorTangentSpace_0);
    var _S26 : f32 = (_S23.worldPosition_0.y + abs(uboFS_0.minHeight_0)) / (abs(uboFS_0.maxHeight_0) + abs(uboFS_0.minHeight_0));
    const _S27 : vec4<f32> = vec4<f32>(1.0f, 1.0f, 1.0f, 1.0f);
    var _S28 : vec2<f32> = dpdx(_S23.textureCoord_0);
    var _S29 : vec2<f32> = dpdy(_S23.textureCoord_0);
    var normal_3 : vec3<f32> = vec3<f32>(0.0f, 1.0f, 0.0f);
    var shineDamper_2 : f32 = 1.0f;
    var reflectivity_2 : f32 = 1.0f;
    var textureColor_0 : vec4<f32> = _S27;
    var i_2 : u32 = u32(0);
    for(;;)
    {
        if(i_2 < u32(4))
        {
        }
        else
        {
            break;
        }
        var uv1_0 : vec2<f32>;
        if(i_2 < u32(3))
        {
            var _S30 : bool;
            if(_S26 > (uboFS_0.heightSteps_0[i_2].x - uboFS_0.heightTransitionRange_0))
            {
                _S30 = _S26 < (uboFS_0.heightSteps_0[i_2].x + uboFS_0.heightTransitionRange_0);
            }
            else
            {
                _S30 = false;
            }
            if(_S30)
            {
                var ratio_0 : f32 = (_S26 - uboFS_0.heightSteps_0[i_2].x + uboFS_0.heightTransitionRange_0) / (2.0f * uboFS_0.heightTransitionRange_0);
                var uv2_0 : vec2<f32>;
                if((uboFS_0.hasConeMap_0) != u32(0))
                {
                    var _S31 : u32 = i_2 + u32(1);
                    var _S32 : vec2<f32> = RelaxedConeStepMapping_0(heightTextures_0, heightSampler_0, _S31, uboFS_0.heightScale_0[_S31].x, uboFS_0.numLayers_0, _S23.textureCoord_0, _S28, _S29, _S25);
                    uv1_0 = RelaxedConeStepMapping_0(heightTextures_0, heightSampler_0, i_2, uboFS_0.heightScale_0[i_2].x, uboFS_0.numLayers_0, _S23.textureCoord_0, _S28, _S29, _S25);
                    uv2_0 = _S32;
                }
                else
                {
                    uv1_0 = _S23.textureCoord_0;
                    uv2_0 = _S23.textureCoord_0;
                }
                if((uboFS_0.hasNormalMap_0) != u32(0))
                {
                    var _S33 : vec3<f32> = sampleNormalMap_0(i_2 + u32(1), uv2_0, _S28, _S29);
                    totalDiffuse_0 = sampleNormalMap_0(i_2, uv1_0, _S28, _S29);
                    totalSpecular_0 = _S33;
                }
                else
                {
                    const _S34 : vec3<f32> = vec3<f32>(0.0f, 0.0f, 1.0f);
                    totalDiffuse_0 = _S34;
                    totalSpecular_0 = _S34;
                }
                var _S35 : u32 = i_2 + u32(1);
                var _S36 : vec4<f32> = mix(sampleColorTextureGrad_0(i_2, uv1_0, _S28, _S29), sampleColorTextureGrad_0(_S35, uv2_0, _S28, _S29), vec4<f32>(ratio_0));
                var _S37 : f32 = mix(uboFS_0.material_0.data_3[i_2].shineDamper_0, uboFS_0.material_0.data_3[_S35].shineDamper_0, ratio_0);
                var _S38 : f32 = mix(uboFS_0.material_0.data_3[i_2].reflectivity_0, uboFS_0.material_0.data_3[_S35].reflectivity_0, ratio_0);
                normal_3 = normalize(mix(totalDiffuse_0, totalSpecular_0, vec3<f32>(ratio_0)));
                shineDamper_2 = _S37;
                reflectivity_2 = _S38;
                textureColor_0 = _S36;
                break;
            }
            else
            {
                if(_S26 < (uboFS_0.heightSteps_0[i_2].x - uboFS_0.heightTransitionRange_0))
                {
                    if((uboFS_0.hasConeMap_0) != u32(0))
                    {
                        uv1_0 = RelaxedConeStepMapping_0(heightTextures_0, heightSampler_0, i_2, uboFS_0.heightScale_0[i_2].x, uboFS_0.numLayers_0, _S23.textureCoord_0, _S28, _S29, _S25);
                    }
                    else
                    {
                        uv1_0 = _S23.textureCoord_0;
                    }
                    if((uboFS_0.hasNormalMap_0) != u32(0))
                    {
                        normal_3 = sampleNormalMap_0(i_2, uv1_0, _S28, _S29);
                    }
                    else
                    {
                        normal_3 = vec3<f32>(0.0f, 0.0f, 1.0f);
                    }
                    var _S39 : vec4<f32> = sampleColorTextureGrad_0(i_2, uv1_0, _S28, _S29);
                    shineDamper_2 = uboFS_0.material_0.data_3[i_2].shineDamper_0;
                    reflectivity_2 = uboFS_0.material_0.data_3[i_2].reflectivity_0;
                    textureColor_0 = _S39;
                    break;
                }
            }
        }
        else
        {
            if((uboFS_0.hasConeMap_0) != u32(0))
            {
                uv1_0 = RelaxedConeStepMapping_0(heightTextures_0, heightSampler_0, i_2, uboFS_0.heightScale_0[i_2].x, uboFS_0.numLayers_0, _S23.textureCoord_0, _S28, _S29, _S25);
            }
            else
            {
                uv1_0 = _S23.textureCoord_0;
            }
            if((uboFS_0.hasNormalMap_0) != u32(0))
            {
                totalDiffuse_0 = sampleNormalMap_0(i_2, uv1_0, _S28, _S29);
            }
            else
            {
                totalDiffuse_0 = vec3<f32>(0.0f, 0.0f, 1.0f);
            }
            var _S40 : vec4<f32> = sampleColorTextureGrad_0(i_2, uv1_0, _S28, _S29);
            normal_3 = totalDiffuse_0;
            shineDamper_2 = uboFS_0.material_0.data_3[i_2].shineDamper_0;
            reflectivity_2 = uboFS_0.material_0.data_3[i_2].reflectivity_0;
            textureColor_0 = _S40;
        }
        i_2 = i_2 + u32(1);
    }
    var shadow_2 : f32;
    if((uboFS_0.castedByShadows_0) != u32(0))
    {
        shadow_2 = GetShadow_0(depthTexture_0, depthSampler_0, _S23.viewPosition_0, _S23.worldPosition_0, 0.00499999988824129f, position_1);
    }
    else
    {
        shadow_2 = 1.0f;
    }
    var _S41 : vec3<f32> = normalize(_S23.toCameraVectorTangentSpace_0 - _S23.positionTangentSpace_0);
    const _S42 : vec3<f32> = vec3<f32>(0.0f, 0.0f, 0.0f);
    var j_1 : u32 = u32(0);
    totalDiffuse_0 = _S42;
    totalSpecular_0 = _S42;
    for(;;)
    {
        if(j_1 < (uboFS_0.lightning_0.realCountOfLights_0))
        {
        }
        else
        {
            break;
        }
        var toLightVector_3 : vec3<f32> = _S24[j_1] - _S23.positionTangentSpace_0;
        var unitToLightVector_0 : vec3<f32> = normalize(toLightVector_3);
        var attenuationFactor_2 : f32 = GetAttenuationFactor_0(uboFS_0.lightning_0.lights_0.data_2[j_1].attenuation_0.xyz, toLightVector_3);
        var _S43 : vec3<f32> = uboFS_0.lightning_0.lights_0.data_2[j_1].color_0.xyz;
        var totalDiffuse_1 : vec3<f32> = totalDiffuse_0 + GetDiffuseColor_0(normal_3, unitToLightVector_0, _S43, attenuationFactor_2);
        var totalSpecular_1 : vec3<f32> = totalSpecular_0 + GetSpecularColor_0(normal_3, unitToLightVector_0, _S41, _S43, attenuationFactor_2, shineDamper_2, reflectivity_2);
        j_1 = j_1 + u32(1);
        totalDiffuse_0 = totalDiffuse_1;
        totalSpecular_0 = totalSpecular_1;
    }
    var resultColor_0 : vec4<f32> = mix(vec4<f32>(uboFS_0.fogColor_0.xyz, 1.0f), vec4<f32>(totalDiffuse_0 * vec3<f32>(shadow_2) + vec3<f32>(uboFS_0.lightning_0.ambientFactor_0), 1.0f) * textureColor_0 + vec4<f32>(totalSpecular_0 * vec3<f32>(shadow_2), 0.0f), vec4<f32>(_S23.visibility_0));
    var resultColor_1 : vec4<f32>;
    if((uboFS_0.selected_0) != u32(0))
    {
        resultColor_1 = mix(resultColor_0, uboFS_0.selectedColor_0, vec4<f32>(0.5f));
    }
    else
    {
        resultColor_1 = resultColor_0;
    }
    var _S44 : pixelOutput_0 = pixelOutput_0( resultColor_1 );
    return _S44;
}

