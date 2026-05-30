@binding(2) @group(0) var colorTexture_0 : texture_2d<f32>;

@binding(3) @group(0) var colorSampler_0 : sampler;

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

struct AnimationFSParams_std140_0
{
    @align(16) shadows_0 : Shadows_std140_0,
    @align(16) lightning_0 : Lightning_std140_0,
    @align(16) material_0 : Material_std140_0,
    @align(16) fogColor_0 : vec4<f32>,
    @align(16) selectedColor_0 : vec4<f32>,
    @align(16) selected_0 : u32,
    @align(4) castedByShadows_0 : u32,
};

@binding(1) @group(0) var<uniform> uboFS_0 : AnimationFSParams_std140_0;
@binding(4) @group(0) var depthTexture_0 : texture_2d_array<f32>;

@binding(5) @group(0) var depthSampler_0 : sampler;

fn GetShadowRawInternal_0( depthTexture_1 : texture_2d_array<f32>,  depthSampler_1 : sampler,  shadowCoord_0 : vec4<f32>,  shadowCoordOffset_0 : vec2<f32>,  cascadeIndex_0 : u32,  depthBias_0 : f32,  useReverseDepth_1 : u32) -> f32
{
    var _S1 : f32 = shadowCoord_0.z;
    var _S2 : bool;
    if(_S1 >= 0.0f)
    {
        _S2 = _S1 <= 1.0f;
    }
    else
    {
        _S2 = false;
    }
    var shadow_0 : f32;
    if(_S2)
    {
        var _S3 : vec3<f32> = vec3<f32>(shadowCoord_0.xy + shadowCoordOffset_0, f32(cascadeIndex_0));
        var depth_0 : f32 = (textureSampleLevel((depthTexture_1), (depthSampler_1), ((_S3)).xy, i32(((_S3)).z), (0.0f))).x;
        if(useReverseDepth_1 != u32(0))
        {
            if(depth_0 > (_S1 + depthBias_0))
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
            if(depth_0 < (_S1 - depthBias_0))
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
    var height_0 : u32;
    var elements_0 : u32;
    {var dim = textureDimensions((depthTexture_2));((width_0)) = dim.x;((height_0)) = dim.y;((elements_0)) = textureNumLayers((depthTexture_2));};
    var texelSize_0 : vec2<f32> = vec2<f32>(1.0f) / vec2<f32>(f32(width_0), f32(height_0));
    var _S4 : vec2<f32> = fract(fragCoord_0.xy * vec2<f32>(0.5f)) + vec2<f32>(0.25f);
    var offset_0 : vec2<f32> = _S4;
    if((_S4.y) > 1.10000002384185791f)
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
    @location(10) normal_2 : vec3<f32>,
    @location(1) worldPosition_0 : vec3<f32>,
    @location(2) viewPosition_0 : vec3<f32>,
    @location(3) toCameraVector_1 : vec3<f32>,
    @location(4) visibility_0 : f32,
    @location(5) toLightVector0_0 : vec3<f32>,
    @location(6) toLightVector1_0 : vec3<f32>,
    @location(7) toLightVector2_0 : vec3<f32>,
    @location(8) toLightVector3_0 : vec3<f32>,
    @location(9) clipDistance_0 : f32,
};

fn GetShadow_0( _S5 : texture_2d_array<f32>,  _S6 : sampler,  _S7 : vec3<f32>,  _S8 : vec3<f32>,  _S9 : f32,  _S10 : vec4<f32>) -> f32
{
    var shadow_1 : f32;
    if((uboFS_0.shadows_0.enabled_0) != u32(0))
    {
        var cascadeIndex_3 : u32 = u32(0);
        var i_0 : u32 = u32(0);
        shadow_1 = _S9;
        for(;;)
        {
            if(i_0 < u32(3))
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
                if((_S7.z) > (uboFS_0.shadows_0.cascades_0.data_1[i_0].split_0.x))
                {
                    var bias_1 : f32 = shadow_1 / 1.5f;
                    cascadeIndex_4 = i_0 + u32(1);
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
                if((_S7.z) < (uboFS_0.shadows_0.cascades_0.data_1[i_0].split_0.x))
                {
                    var bias_2 : f32 = shadow_1 / 1.5f;
                    cascadeIndex_4 = i_0 + u32(1);
                    bias_0 = bias_2;
                }
                else
                {
                    cascadeIndex_4 = cascadeIndex_3;
                    bias_0 = shadow_1;
                }
            }
            var _S11 : u32 = i_0 + u32(1);
            cascadeIndex_3 = cascadeIndex_4;
            i_0 = _S11;
            shadow_1 = bias_0;
        }
        var shadowCoord_3 : vec4<f32> = (((vec4<f32>(_S8, 1.0f)) * (mat4x4<f32>(uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(0)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(1)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(2)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(0)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(1)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(2)][i32(3)], uboFS_0.shadows_0.cascades_0.data_1[cascadeIndex_3].viewProjectionMatrix_0.data_0[i32(3)][i32(3)]))));
        shadow_1 = GetShadowSingle_0(_S5, _S6, shadowCoord_3 / vec4<f32>(shadowCoord_3.w), cascadeIndex_3, shadow_1, uboFS_0.shadows_0.useReverseDepth_0, _S10);
    }
    else
    {
        shadow_1 = 1.0f;
    }
    return shadow_1;
}

@fragment
fn fragmentMain( _S12 : pixelInput_0, @builtin(position) position_1 : vec4<f32>) -> pixelOutput_0
{
    var _S13 : array<vec3<f32>, i32(4)> = array<vec3<f32>, i32(4)>( _S12.toLightVector0_0, _S12.toLightVector1_0, _S12.toLightVector2_0, _S12.toLightVector3_0 );
    if((_S12.clipDistance_0) < 0.0f)
    {
        discard;
    }
    var textureColor_0 : vec4<f32> = (textureSample((colorTexture_0), (colorSampler_0), (_S12.textureCoord_0)));
    var shadow_2 : f32;
    if((uboFS_0.castedByShadows_0) != u32(0))
    {
        shadow_2 = GetShadow_0(depthTexture_0, depthSampler_0, _S12.viewPosition_0, _S12.worldPosition_0, 0.00499999988824129f, position_1);
    }
    else
    {
        shadow_2 = 1.0f;
    }
    var _S14 : vec3<f32> = normalize(_S12.normal_2);
    var _S15 : vec3<f32> = normalize(_S12.toCameraVector_1);
    var _S16 : vec3<f32> = vec3<f32>(0.0f);
    var i_1 : u32 = u32(0);
    var totalDiffuse_0 : vec3<f32> = _S16;
    var totalSpecular_0 : vec3<f32> = _S16;
    for(;;)
    {
        if(i_1 < (uboFS_0.lightning_0.realCountOfLights_0))
        {
        }
        else
        {
            break;
        }
        var unitToLightVector_0 : vec3<f32> = normalize(_S13[i_1]);
        var attenuationFactor_2 : f32 = GetAttenuationFactor_0(uboFS_0.lightning_0.lights_0.data_2[i_1].attenuation_0.xyz, _S13[i_1]);
        var _S17 : vec3<f32> = uboFS_0.lightning_0.lights_0.data_2[i_1].color_0.xyz;
        var totalDiffuse_1 : vec3<f32> = totalDiffuse_0 + GetDiffuseColor_0(_S14, unitToLightVector_0, _S17, attenuationFactor_2);
        var totalSpecular_1 : vec3<f32> = totalSpecular_0 + GetSpecularColor_0(_S14, unitToLightVector_0, _S15, _S17, attenuationFactor_2, uboFS_0.material_0.shineDamper_0, uboFS_0.material_0.reflectivity_0);
        i_1 = i_1 + u32(1);
        totalDiffuse_0 = totalDiffuse_1;
        totalSpecular_0 = totalSpecular_1;
    }
    var resultColor_0 : vec4<f32> = mix(vec4<f32>(uboFS_0.fogColor_0.xyz, 1.0f), vec4<f32>(totalDiffuse_0 * vec3<f32>(shadow_2) + vec3<f32>(uboFS_0.lightning_0.ambientFactor_0), 1.0f) * textureColor_0 + vec4<f32>(totalSpecular_0 * vec3<f32>(shadow_2), 0.0f), vec4<f32>(_S12.visibility_0));
    var resultColor_1 : vec4<f32>;
    if((uboFS_0.selected_0) != u32(0))
    {
        resultColor_1 = mix(resultColor_0, uboFS_0.selectedColor_0, vec4<f32>(0.5f));
    }
    else
    {
        resultColor_1 = resultColor_0;
    }
    var _S18 : pixelOutput_0 = pixelOutput_0( resultColor_1 );
    return _S18;
}

