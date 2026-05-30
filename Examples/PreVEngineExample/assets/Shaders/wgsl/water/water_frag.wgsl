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

struct WaterLight_std140_0
{
    @align(16) position_0 : vec4<f32>,
    @align(16) color_0 : vec4<f32>,
};

struct WaterFSParams_std140_0
{
    @align(16) shadows_0 : Shadows_std140_0,
    @align(16) fogColor_0 : vec4<f32>,
    @align(16) waterColor_0 : vec4<f32>,
    @align(16) light_0 : WaterLight_std140_0,
    @align(16) nearFarClippinPlane_0 : vec4<f32>,
    @align(16) viewportSize_0 : vec4<f32>,
    @align(16) moveFactor_0 : f32,
};

@binding(1) @group(0) var<uniform> uboFS_0 : WaterFSParams_std140_0;
@binding(2) @group(0) var depthTexture_0 : texture_2d_array<f32>;

@binding(3) @group(0) var depthSampler_0 : sampler;

@binding(8) @group(0) var depthMapTexture_0 : texture_2d<f32>;

@binding(9) @group(0) var depthMapSampler_0 : sampler;

@binding(10) @group(0) var dudvMapTexture_0 : texture_2d<f32>;

@binding(11) @group(0) var dudvMapSampler_0 : sampler;

@binding(4) @group(0) var reflectionTexture_0 : texture_2d<f32>;

@binding(5) @group(0) var reflectionSampler_0 : sampler;

@binding(6) @group(0) var refractionTexture_0 : texture_2d<f32>;

@binding(7) @group(0) var refractionSampler_0 : sampler;

@binding(12) @group(0) var normalMapTexture_0 : texture_2d<f32>;

@binding(13) @group(0) var normalMapSampler_0 : sampler;

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

fn LinearizeDepth_0( depth_1 : f32,  zNear_0 : f32,  zFar_0 : f32) -> f32
{
    return zNear_0 * zFar_0 / (zFar_0 + depth_1 * (zNear_0 - zFar_0));
}

struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

struct pixelInput_0
{
    @location(0) clipSpaceCoord_0 : vec4<f32>,
    @location(1) textureCoord_0 : vec2<f32>,
    @location(2) worldPosition_0 : vec3<f32>,
    @location(3) viewPosition_0 : vec3<f32>,
    @location(4) toCameraVector_0 : vec3<f32>,
    @location(5) visibility_0 : f32,
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
    var _S13 : f32 = GetShadow_0(depthTexture_0, depthSampler_0, _S12.viewPosition_0, _S12.worldPosition_0, 0.00499999988824129f, position_1);
    var shadow_2 : f32;
    if(_S13 < 0.99900001287460327f)
    {
        shadow_2 = 0.0f;
    }
    else
    {
        shadow_2 = _S13;
    }
    var normalizedDeviceSpaceCoord_0 : vec2<f32> = position_1.xy / uboFS_0.viewportSize_0.xy;
    var _S14 : f32 = normalizedDeviceSpaceCoord_0.x;
    var _S15 : f32 = normalizedDeviceSpaceCoord_0.y;
    var refractTexCoord_0 : vec2<f32> = vec2<f32>(_S14, _S15);
    var waterDepth_0 : f32 = LinearizeDepth_0((textureSample((depthMapTexture_0), (depthMapSampler_0), (refractTexCoord_0))).x, uboFS_0.nearFarClippinPlane_0.x, uboFS_0.nearFarClippinPlane_0.y) - LinearizeDepth_0(position_1.z, uboFS_0.nearFarClippinPlane_0.x, uboFS_0.nearFarClippinPlane_0.y);
    var distortedTexCoords_0 : vec2<f32> = (textureSample((dudvMapTexture_0), (dudvMapSampler_0), (vec2<f32>(_S12.textureCoord_0.x + uboFS_0.moveFactor_0, _S12.textureCoord_0.y)))).xy * vec2<f32>(0.10000000149011612f);
    var distortedTexCoords_1 : vec2<f32> = _S12.textureCoord_0 + vec2<f32>(distortedTexCoords_0.x, distortedTexCoords_0.y + uboFS_0.moveFactor_0);
    var totalDistortion_0 : vec2<f32> = ((textureSample((dudvMapTexture_0), (dudvMapSampler_0), (distortedTexCoords_1))).xy * vec2<f32>(2.0f) - vec2<f32>(1.0f)) * vec2<f32>(0.03999999910593033f) * vec2<f32>(clamp(waterDepth_0 / 12.0f, 0.0f, 1.0f));
    var _S16 : vec2<f32> = vec2<f32>(0.00100000004749745f);
    var _S17 : vec2<f32> = vec2<f32>(0.99900001287460327f);
    var normalMapColor_0 : vec4<f32> = (textureSample((normalMapTexture_0), (normalMapSampler_0), (distortedTexCoords_1)));
    var normal_0 : vec3<f32> = normalize(vec3<f32>(normalMapColor_0.x * 2.0f - 1.0f, normalMapColor_0.y * 3.0f, normalMapColor_0.z * 2.0f - 1.0f));
    var viewVector_0 : vec3<f32> = normalize(_S12.toCameraVector_0);
    var outColor_0 : vec4<f32> = clamp(mix(mix((textureSample((reflectionTexture_0), (reflectionSampler_0), (clamp(vec2<f32>(_S14, 1.0f - _S15) + totalDistortion_0, _S16, _S17)))), (textureSample((refractionTexture_0), (refractionSampler_0), (clamp(refractTexCoord_0 + totalDistortion_0, _S16, _S17)))), vec4<f32>(clamp(pow(dot(viewVector_0, normal_0), 0.69999998807907104f), 0.00100000004749745f, 0.99900001287460327f))), uboFS_0.waterColor_0 * vec4<f32>(shadow_2), vec4<f32>(0.20000000298023224f)) + vec4<f32>(uboFS_0.light_0.color_0.xyz * vec3<f32>(pow(max(dot(reflect(normalize((vec3<f32>(0) - uboFS_0.light_0.position_0.xyz - _S12.worldPosition_0.xyz)), normal_0), viewVector_0), 0.0f), 20.0f)) * vec3<f32>(0.44999998807907104f) * vec3<f32>(clamp(waterDepth_0 / 5.0f, 0.0f, 1.0f)), 1.0f), vec4<f32>(0.0f), vec4<f32>(1.0f));
    outColor_0[i32(3)] = clamp(waterDepth_0 / 5.0f, 0.0f, 1.0f);
    var _S18 : pixelOutput_0 = pixelOutput_0( outColor_0 );
    return _S18;
}

