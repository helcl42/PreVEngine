struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct SkyCloudParams_std140_0
{
    @align(16) resolution_0 : vec4<f32>,
    @align(16) projectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) inverseProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) viewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) inverseViewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) lightColor_0 : vec4<f32>,
    @align(16) lightDirection_0 : vec4<f32>,
    @align(16) baseCloudColor_0 : vec4<f32>,
    @align(16) skyColorBottom_0 : vec4<f32>,
    @align(16) skyColorTop_0 : vec4<f32>,
    @align(16) cameraPosition_0 : vec4<f32>,
    @align(16) worldOrigin_0 : vec4<f32>,
    @align(16) windDirection_0 : vec4<f32>,
    @align(16) earthRadius_0 : f32,
    @align(4) sphereInnerRadius_0 : f32,
    @align(8) sphereOuterRadius_0 : f32,
    @align(4) cloudTopOffset_0 : f32,
    @align(16) coverageFactor_0 : f32,
    @align(4) crispiness_0 : f32,
    @align(8) curliness_0 : f32,
    @align(4) densityFactor_0 : f32,
    @align(16) cloudSpeed_0 : f32,
    @align(4) absorption_0 : f32,
    @align(8) maxDepth_0 : f32,
    @align(4) time_0 : f32,
    @align(16) cloudSteps_0 : u32,
    @align(4) lightSteps_0 : u32,
    @align(8) useIGN_0 : u32,
    @align(4) frameCounter_0 : u32,
    @align(16) enablePowder_0 : u32,
    @align(4) powderWeight_0 : f32,
    @align(8) lodScale_0 : f32,
    @align(4) ambientScale_0 : f32,
    @align(16) prevViewProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) currentViewProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) prevInverseViewProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) enableCheckerboard_0 : u32,
    @align(4) reprojectionBlend_0 : f32,
    @align(8) enableTemporalBlend_0 : u32,
    @align(4) enableFullReproject_0 : u32,
};

@binding(8) @group(0) var<uniform> uboCS_0 : SkyCloudParams_std140_0;
@binding(0) @group(0) var outFragColor_0 : texture_storage_2d<rgba8unorm, write>;

@binding(1) @group(0) var outBloom_0 : texture_storage_2d<rgba8unorm, write>;

@binding(2) @group(0) var outAlphaness_0 : texture_storage_2d<rgba8unorm, write>;

@binding(3) @group(0) var outCloudDistance_0 : texture_storage_2d<r32float, write>;

@binding(11) @group(0) var prevDepthTex_0 : texture_2d<f32>;

@binding(12) @group(0) var prevDepthSampler_0 : sampler;

@binding(9) @group(0) var prevColorTex_0 : texture_2d<f32>;

@binding(10) @group(0) var prevColorSampler_0 : sampler;

@binding(4) @group(0) var perlinNoiseTex_0 : texture_3d<f32>;

@binding(5) @group(0) var perlinNoiseSampler_0 : sampler;

@binding(6) @group(0) var weatherTex_0 : texture_2d<f32>;

@binding(7) @group(0) var weatherSampler_0 : sampler;

const noiseKernel_0 : array<vec3<f32>, i32(6)> = array<vec3<f32>, i32(6)>( vec3<f32>(0.38051304221153259f, 0.92453449964523315f, -0.0211134497076273f), vec3<f32>(-0.50625801086425781f, -0.03590792044997215f, -0.86163419485092163f), vec3<f32>(-0.32509216666221619f, -0.94557440280914307f, 0.01428792998194695f), vec3<f32>(0.09026238322257996f, -0.2737654447555542f, 0.95755165815353394f), vec3<f32>(0.28128597140312195f, 0.42443639039993286f, -0.86065787076950073f), vec3<f32>(-0.16852402687072754f, 0.14748696982860565f, 0.97460103034973145f) );
const bayerFilter_0 : array<f32, i32(16)> = array<f32, i32(16)>( 0.0f, 0.5f, 0.125f, 0.625f, 0.75f, 0.25f, 0.875f, 0.375f, 0.1875f, 0.6875f, 0.0625f, 0.5625f, 0.9375f, 0.4375f, 0.8125f, 0.3125f );
fn ComputeClipSpaceDirection_0( fragCoord_0 : vec2<i32>) -> vec4<f32>
{
    return vec4<f32>(vec2<f32>(2.0f) * (vec2<f32>(fragCoord_0.xy) / uboCS_0.resolution_0.xy) - vec2<f32>(1.0f), 1.0f, 1.0f);
}

fn ComputeViewSpaceDirection_0( clipSpaceDir_0 : vec4<f32>) -> vec4<f32>
{
    return vec4<f32>((((clipSpaceDir_0) * (mat4x4<f32>(uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(3)])))).xy, -1.0f, 0.0f);
}

fn ComputeWorldSpaceDirection_0( viewSpaceDir_0 : vec4<f32>) -> vec3<f32>
{
    return normalize((((viewSpaceDir_0) * (mat4x4<f32>(uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(3)])))).xyz);
}

fn GetCubeMapColor_0( worldRayDirection_0 : vec3<f32>) -> vec3<f32>
{
    return mix(uboCS_0.skyColorBottom_0.xyz, uboCS_0.skyColorTop_0.xyz, vec3<f32>(smoothstep(-0.07000000029802322f, 0.07000000029802322f, worldRayDirection_0.y)));
}

fn GetSunColor_0( worldRayDirection_1 : vec3<f32>,  powExp_0 : f32) -> vec3<f32>
{
    return vec3<f32>(0.80000001192092896f) * vec3<f32>(1.0f, 0.60000002384185791f, 0.10000000149011612f) * vec3<f32>(pow(clamp(dot(uboCS_0.lightDirection_0.xyz, worldRayDirection_1), 0.0f, 1.0f), powExp_0));
}

fn GetBackgroundColor_0( worldRayDirection_2 : vec3<f32>) -> vec4<f32>
{
    return vec4<f32>(GetCubeMapColor_0(worldRayDirection_2) + GetSunColor_0(worldRayDirection_2, 350.0f), 1.0f);
}

fn GetEarthCenter_0() -> vec3<f32>
{
    return uboCS_0.worldOrigin_0.xyz + vec3<f32>(0.0f, - uboCS_0.earthRadius_0, 0.0f);
}

fn RaySphereIntersection_0( rayStartPosition_0 : vec3<f32>,  rayDirection_0 : vec3<f32>,  sphereCenter_0 : vec3<f32>,  radius_0 : f32,  intersectionPoint_0 : ptr<function, vec3<f32>>) -> bool
{
    var L_0 : vec3<f32> = rayStartPosition_0 - sphereCenter_0;
    var a_0 : f32 = dot(rayDirection_0, rayDirection_0);
    var b_0 : f32 = 2.0f * dot(rayDirection_0, L_0);
    var discr_0 : f32 = b_0 * b_0 - 4.0f * a_0 * (dot(L_0, L_0) - radius_0 * radius_0);
    if(discr_0 < 0.0f)
    {
        (*intersectionPoint_0) = vec3<f32>(0.0f);
        return false;
    }
    var sqrtDiscr_0 : f32 = sqrt(discr_0);
    var _S1 : f32 = - b_0;
    var _S2 : f32 = 2.0f * a_0;
    var t1_0 : f32 = (_S1 - sqrtDiscr_0) / _S2;
    var t2_0 : f32 = (_S1 + sqrtDiscr_0) / _S2;
    var t_0 : f32;
    if(t1_0 > 0.0f)
    {
        t_0 = t1_0;
    }
    else
    {
        t_0 = t2_0;
    }
    if(t_0 <= 0.0f)
    {
        (*intersectionPoint_0) = vec3<f32>(0.0f);
        return false;
    }
    (*intersectionPoint_0) = rayStartPosition_0 + rayDirection_0 * vec3<f32>(t_0);
    return true;
}

fn ComputeFogAmount_0( startPos_0 : vec3<f32>,  factor_0 : f32) -> f32
{
    var dist_0 : f32 = length(startPos_0 - uboCS_0.cameraPosition_0.xyz);
    return 1.0f - exp(- dist_0 * (dist_0 / ((uboCS_0.cameraPosition_0.y - GetEarthCenter_0().y) * 0.30000001192092896f)) * factor_0);
}

fn InterleavedGradientNoise_0( fragCoord_1 : vec2<f32>) -> f32
{
    return fract(52.98291778564453125f * fract(dot(fragCoord_1, vec2<f32>(0.06711056083440781f, 0.00583714991807938f))));
}

fn BayerDither_0( fragCoord_2 : vec2<u32>) -> f32
{
    return bayerFilter_0[i32(fragCoord_2.x) % i32(4) * i32(4) + i32(fragCoord_2.y) % i32(4)];
}

fn GetDitherOffset_0( fragCoord_3 : vec2<u32>) -> f32
{
    if((uboCS_0.useIGN_0) != u32(0))
    {
        return InterleavedGradientNoise_0(vec2<f32>(fragCoord_3));
    }
    else
    {
        return BayerDither_0(fragCoord_3);
    }
}

fn GetHeightFraction_0( inPos_0 : vec3<f32>) -> f32
{
    return (length(inPos_0 - GetEarthCenter_0()) - uboCS_0.sphereInnerRadius_0) / (uboCS_0.sphereOuterRadius_0 - uboCS_0.sphereInnerRadius_0);
}

fn GetUVProjection_0( inPos_1 : vec3<f32>) -> vec2<f32>
{
    return inPos_1.xz / vec2<f32>(uboCS_0.sphereInnerRadius_0) + vec2<f32>(0.5f);
}

fn Remap_0( originalValue_0 : f32,  originalMin_0 : f32,  originalMax_0 : f32,  newMin_0 : f32,  newMax_0 : f32) -> f32
{
    return newMin_0 + (originalValue_0 - originalMin_0) / (originalMax_0 - originalMin_0) * (newMax_0 - newMin_0);
}

fn GetDensityForCloud_0( heightFraction_0 : f32,  cloudType_0 : f32) -> f32
{
    var _S3 : f32 = cloudType_0 - 0.5f;
    var baseGradient_0 : vec4<f32> = vec4<f32>((1.0f - clamp(cloudType_0 * 2.0f, 0.0f, 1.0f))) * vec4<f32>(0.0f, 0.10000000149011612f, 0.20000000298023224f, 0.30000001192092896f) + vec4<f32>((1.0f - abs(_S3) * 2.0f)) * vec4<f32>(0.01999999955296516f, 0.20000000298023224f, 0.47999998927116394f, 0.625f) + vec4<f32>((clamp(_S3, 0.0f, 1.0f) * 2.0f)) * vec4<f32>(0.0f, 0.16249999403953552f, 0.87999999523162842f, 0.98000001907348633f);
    return smoothstep(baseGradient_0.x, baseGradient_0.y, heightFraction_0) - smoothstep(baseGradient_0.z, baseGradient_0.w, heightFraction_0);
}

fn SampleCloudDensity_0( inPos_2 : vec3<f32>,  expensive_0 : bool,  lod_0 : f32) -> f32
{
    var heightFraction_1 : f32 = GetHeightFraction_0(inPos_2);
    var uv_0 : vec2<f32> = GetUVProjection_0(inPos_2);
    var movingUV_0 : vec2<f32> = GetUVProjection_0(inPos_2 + (vec3<f32>(heightFraction_1) * uboCS_0.windDirection_0.xyz * vec3<f32>(uboCS_0.cloudTopOffset_0) + uboCS_0.windDirection_0.xyz * vec3<f32>(uboCS_0.time_0) * vec3<f32>(uboCS_0.cloudSpeed_0)));
    var _S4 : bool;
    if(heightFraction_1 < 0.0f)
    {
        _S4 = true;
    }
    else
    {
        _S4 = heightFraction_1 > 1.0f;
    }
    if(_S4)
    {
        return 0.0f;
    }
    const noiseScaler_0 : vec3<f32> = vec3<f32>(0.625f, 0.25f, 0.125f);
    var lowFrequencyNoise_0 : vec4<f32> = (textureSampleLevel((perlinNoiseTex_0), (perlinNoiseSampler_0), (vec3<f32>(uv_0 * vec2<f32>(uboCS_0.crispiness_0), heightFraction_1)), (lod_0)));
    var cloudCoverage_0 : f32 = (textureSampleLevel((weatherTex_0), (weatherSampler_0), (movingUV_0), (0.0f))).xyz.x * uboCS_0.coverageFactor_0;
    var baseCloudWithCoverage_0 : f32 = Remap_0(Remap_0(lowFrequencyNoise_0.x, - (1.0f - dot(lowFrequencyNoise_0.yzw, noiseScaler_0)), 1.0f, 0.0f, 1.0f) * (GetDensityForCloud_0(heightFraction_1, 1.0f) / max(heightFraction_1, 0.00100000004749745f)), cloudCoverage_0, 1.0f, 0.0f, 1.0f) * cloudCoverage_0;
    var baseCloudWithCoverage_1 : f32;
    if(expensive_0)
    {
        var highFreqFBM_0 : f32 = dot((textureSampleLevel((perlinNoiseTex_0), (perlinNoiseSampler_0), (vec3<f32>(movingUV_0 * vec2<f32>(uboCS_0.crispiness_0) * vec2<f32>(uboCS_0.curliness_0), heightFraction_1)), (lod_0))).xyz.xyz, noiseScaler_0);
        var highFreqNoiseModifier_0 : f32 = mix(highFreqFBM_0, 1.0f - highFreqFBM_0, clamp(heightFraction_1 * 10.0f, 0.0f, 1.0f));
        baseCloudWithCoverage_1 = Remap_0((baseCloudWithCoverage_0 - highFreqNoiseModifier_0 * (1.0f - baseCloudWithCoverage_0)) * 2.0f, highFreqNoiseModifier_0 * 0.20000000298023224f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        baseCloudWithCoverage_1 = baseCloudWithCoverage_0;
    }
    return clamp(baseCloudWithCoverage_1, 0.0f, 1.0f);
}

fn RaymarchToLight_0( inPos_3 : vec3<f32>,  stepSize_0 : f32,  lightDir_0 : vec3<f32>,  originalDensity_0 : f32,  lightDotEye_0 : f32) -> f32
{
    var T_0 : f32;
    var lightSteps_1 : i32 = i32(uboCS_0.lightSteps_0);
    var _S5 : f32 = f32(lightSteps_1);
    var ds_0 : f32 = stepSize_0 * _S5;
    var _S6 : vec3<f32> = lightDir_0 * vec3<f32>(ds_0);
    var _S7 : f32 = 1.0f / _S5;
    var _S8 : f32 = - ds_0 * uboCS_0.absorption_0;
    var i_0 : i32 = i32(0);
    var startPos_1 : vec3<f32> = inPos_3;
    var coneRadius_0 : f32 = 1.0f;
    var density_0 : f32 = 0.0f;
    var T_1 : f32 = 1.0f;
    for(;;)
    {
        if(i_0 < lightSteps_1)
        {
        }
        else
        {
            T_0 = T_1;
            break;
        }
        var _S9 : f32 = f32(i_0);
        var pos_0 : vec3<f32> = startPos_1 + vec3<f32>(coneRadius_0) * noiseKernel_0[i_0 % i32(6)] * vec3<f32>(_S9);
        if((GetHeightFraction_0(pos_0)) >= 0.0f)
        {
            var cloudDensity_0 : f32 = SampleCloudDensity_0(pos_0, density_0 > 0.30000001192092896f, _S9 / _S5);
            var density_1 : f32;
            if(cloudDensity_0 > 0.0f)
            {
                var density_2 : f32 = density_0 + cloudDensity_0;
                T_0 = T_1 * exp(cloudDensity_0 * _S8);
                density_1 = density_2;
            }
            else
            {
                T_0 = T_1;
                density_1 = density_0;
            }
            density_0 = density_1;
        }
        else
        {
            T_0 = T_1;
        }
        if(T_0 < 0.00999999977648258f)
        {
            break;
        }
        var startPos_2 : vec3<f32> = startPos_1 + _S6;
        var coneRadius_1 : f32 = coneRadius_0 + _S7;
        i_0 = i_0 + i32(1);
        startPos_1 = startPos_2;
        coneRadius_0 = coneRadius_1;
        T_1 = T_0;
    }
    return T_0;
}

fn HG_0( sunDotRayDirection_0 : f32,  g_0 : f32) -> f32
{
    var gg_0 : f32 = g_0 * g_0;
    return (1.0f - gg_0) / pow(1.0f + gg_0 - 2.0f * g_0 * sunDotRayDirection_0, 1.5f);
}

fn GetPowder_0( density_3 : f32) -> f32
{
    return 1.0f - exp(-2.0f * density_3);
}

struct RaymarchToCloudResult_0
{
     hit_0 : bool,
     position_0 : vec3<f32>,
     color_0 : vec4<f32>,
};

fn RaymarchToCloud_0( startPos_3 : vec3<f32>,  endPos_0 : vec3<f32>,  bg_0 : vec3<f32>,  fragCoord_4 : vec2<u32>) -> RaymarchToCloudResult_0
{
    var countOfSteps_0 : i32 = i32(uboCS_0.cloudSteps_0);
    var path_0 : vec3<f32> = endPos_0 - startPos_3;
    var stepVal_0 : f32 = length(path_0) / f32(countOfSteps_0);
    var dir_0 : vec3<f32> = normalize(path_0);
    var stepVector_0 : vec3<f32> = dir_0 * vec3<f32>(stepVal_0);
    var _S10 : vec3<f32> = startPos_3 + stepVector_0 * vec3<f32>(GetDitherOffset_0(fragCoord_4));
    var _S11 : f32 = dot(normalize(uboCS_0.lightDirection_0.xyz), dir_0);
    var _S12 : f32 = - stepVal_0 * uboCS_0.densityFactor_0;
    var result_0 : RaymarchToCloudResult_0;
    result_0.hit_0 = false;
    result_0.position_0 = vec3<f32>(9.99999995904e+11f, 9.99999995904e+11f, 9.99999995904e+11f);
    result_0.color_0 = vec4<f32>(0.0f);
    var _S13 : f32 = uboCS_0.sphereOuterRadius_0 - uboCS_0.sphereInnerRadius_0;
    var i_1 : i32 = i32(0);
    var pos_1 : vec3<f32> = _S10;
    var T_2 : f32 = 1.0f;
    for(;;)
    {
        if(i_1 < countOfSteps_0)
        {
        }
        else
        {
            break;
        }
        var densitySample_0 : f32 = SampleCloudDensity_0(pos_1, true, clamp(f32(i_1) * stepVal_0 / (_S13 * uboCS_0.lodScale_0), 0.0f, 1.0f));
        var T_3 : f32;
        if(densitySample_0 > 0.0f)
        {
            if(!result_0.hit_0)
            {
                result_0.position_0 = pos_1;
                result_0.hit_0 = true;
            }
            var ambientLight_0 : vec3<f32> = uboCS_0.baseCloudColor_0.xyz * vec3<f32>(mix(0.80000001192092896f, 1.0f, GetHeightFraction_0(pos_1))) * vec3<f32>(uboCS_0.ambientScale_0);
            var _S14 : f32 = max(RaymarchToLight_0(pos_1, stepVal_0 * 0.10000000149011612f, uboCS_0.lightDirection_0.xyz, densitySample_0, _S11), (1.0f - 0.5f * exp(- densitySample_0 * 2.0f)) * 0.15000000596046448f);
            var _S15 : f32 = clamp(_S11 * 0.5f + 0.5f, 0.0f, 1.0f);
            var _S16 : f32 = max(mix(HG_0(_S11, -0.07999999821186066f), HG_0(_S11, 0.07999999821186066f), _S15), 0.03500000014901161f);
            if((uboCS_0.enablePowder_0) == u32(1))
            {
                T_3 = 1.0f - _S15 * (1.0f - GetPowder_0(densitySample_0)) * uboCS_0.powderWeight_0;
            }
            else
            {
                T_3 = 1.0f;
            }
            var S_0 : vec3<f32> = vec3<f32>(0.69999998807907104f) * mix(mix(ambientLight_0, bg_0, vec3<f32>(0.20000000298023224f)), vec3<f32>(_S16) * uboCS_0.lightColor_0.xyz, vec3<f32>(_S14)) * vec3<f32>(densitySample_0) * vec3<f32>(T_3);
            var dTrans_0 : f32 = exp(densitySample_0 * _S12);
            var _S17 : vec3<f32> = result_0.color_0.xyz + vec3<f32>(T_2) * ((S_0 - S_0 * vec3<f32>(dTrans_0)) * vec3<f32>((1.0f / densitySample_0)));
            result_0.color_0.x = _S17.x;
            result_0.color_0.y = _S17.y;
            result_0.color_0.z = _S17.z;
            T_3 = T_2 * dTrans_0;
        }
        else
        {
            T_3 = T_2;
        }
        if(T_3 <= 0.10000000149011612f)
        {
            T_2 = T_3;
            break;
        }
        var pos_2 : vec3<f32> = pos_1 + stepVector_0;
        i_1 = i_1 + i32(1);
        pos_1 = pos_2;
        T_2 = T_3;
    }
    result_0.color_0[i32(3)] = 1.0f - T_2;
    return result_0;
}

fn ComputeDepth_0( projectionMatrix_1 : mat4x4<f32>,  viewMatrix_1 : mat4x4<f32>,  pointInWorldSpace_0 : vec3<f32>) -> f32
{
    var pInClipSpace_0 : vec4<f32> = ((((((vec4<f32>(pointInWorldSpace_0, 1.0f)) * (viewMatrix_1)))) * (projectionMatrix_1)));
    return (pInClipSpace_0.xyz / vec3<f32>(pInClipSpace_0.w)).z;
}

@compute
@workgroup_size(16, 16, 1)
fn computeMain(@builtin(global_invocation_id) dispatchThreadID_0 : vec3<u32>)
{
    var reproject1_0 : bool;
    var reproject2_0 : bool;
    var _S18 : vec2<i32> = vec2<i32>(dispatchThreadID_0.xy);
    var worldDir_0 : vec3<f32> = ComputeWorldSpaceDirection_0(ComputeViewSpaceDirection_0(ComputeClipSpaceDirection_0(_S18)));
    var backgroundColor_0 : vec4<f32> = GetBackgroundColor_0(worldDir_0);
    var sphereCenter_1 : vec3<f32> = GetEarthCenter_0();
    var startPos_4 : vec3<f32>;
    var endPos_1 : vec3<f32>;
    var fogRay_0 : vec3<f32>;
    var camDistFromCenter_0 : f32 = length(uboCS_0.cameraPosition_0.xyz - sphereCenter_1);
    if(camDistFromCenter_0 < (uboCS_0.sphereOuterRadius_0))
    {
        startPos_4 = uboCS_0.cameraPosition_0.xyz;
        var _S19 : bool = RaySphereIntersection_0(uboCS_0.cameraPosition_0.xyz, worldDir_0, sphereCenter_1, uboCS_0.sphereOuterRadius_0, &(endPos_1));
        if(camDistFromCenter_0 < (uboCS_0.sphereInnerRadius_0))
        {
            var _S20 : bool = RaySphereIntersection_0(uboCS_0.cameraPosition_0.xyz, worldDir_0, sphereCenter_1, uboCS_0.sphereInnerRadius_0, &(fogRay_0));
            startPos_4 = mix(uboCS_0.cameraPosition_0.xyz, fogRay_0, vec3<f32>(clamp((uboCS_0.sphereInnerRadius_0 - camDistFromCenter_0) / ((uboCS_0.sphereOuterRadius_0 - uboCS_0.sphereInnerRadius_0) * 0.10000000149011612f), 0.0f, 1.0f)));
        }
        else
        {
            fogRay_0 = uboCS_0.cameraPosition_0.xyz;
        }
    }
    else
    {
        var hitOuter_0 : bool = RaySphereIntersection_0(uboCS_0.cameraPosition_0.xyz, worldDir_0, sphereCenter_1, uboCS_0.sphereOuterRadius_0, &(startPos_4));
        if(!hitOuter_0)
        {
            var _S21 : vec2<u32> = vec2<u32>(_S18);
            var _S22 : vec4<f32> = vec4<f32>(0.0f);
            textureStore((outFragColor_0), (_S21), (_S22));
            textureStore((outBloom_0), (_S21), (vec4<f32>(GetSunColor_0(worldDir_0, 128.0f) * vec3<f32>(1.29999995231628418f), 1.0f)));
            textureStore((outAlphaness_0), (_S21), (_S22));
            textureStore((outCloudDistance_0), (_S21), (vec4<f32>(uboCS_0.maxDepth_0, 0.0f, 0.0f, 0.0f)));
            return;
        }
        var hitInner_0 : bool = RaySphereIntersection_0(uboCS_0.cameraPosition_0.xyz, worldDir_0, sphereCenter_1, uboCS_0.sphereInnerRadius_0, &(endPos_1));
        if(!hitInner_0)
        {
            var L_1 : vec3<f32> = uboCS_0.cameraPosition_0.xyz - sphereCenter_1;
            var a_1 : f32 = dot(worldDir_0, worldDir_0);
            var b_1 : f32 = 2.0f * dot(worldDir_0, L_1);
            endPos_1 = uboCS_0.cameraPosition_0.xyz + worldDir_0 * vec3<f32>(((- b_1 + sqrt(b_1 * b_1 - 4.0f * a_1 * (dot(L_1, L_1) - uboCS_0.sphereOuterRadius_0 * uboCS_0.sphereOuterRadius_0))) / (2.0f * a_1)));
        }
        fogRay_0 = startPos_4;
    }
    var fogAmount_0 : f32 = ComputeFogAmount_0(fogRay_0, 0.00005999999848427f);
    var fogAmount_1 : f32;
    if(camDistFromCenter_0 < (uboCS_0.sphereOuterRadius_0))
    {
        fogAmount_1 = fogAmount_0 * clamp((uboCS_0.sphereInnerRadius_0 - camDistFromCenter_0) / ((uboCS_0.sphereOuterRadius_0 - uboCS_0.sphereInnerRadius_0) * 0.10000000149011612f), 0.0f, 1.0f);
    }
    else
    {
        fogAmount_1 = fogAmount_0;
    }
    if(fogAmount_1 > 0.9649999737739563f)
    {
        var _S23 : vec2<u32> = vec2<u32>(_S18);
        var _S24 : vec4<f32> = vec4<f32>(0.0f);
        textureStore((outFragColor_0), (_S23), (_S24));
        textureStore((outBloom_0), (_S23), (vec4<f32>(GetSunColor_0(worldDir_0, 128.0f) * vec3<f32>(1.29999995231628418f), 1.0f)));
        textureStore((outAlphaness_0), (_S23), (_S24));
        textureStore((outCloudDistance_0), (_S23), (vec4<f32>(uboCS_0.maxDepth_0, 0.0f, 0.0f, 0.0f)));
        return;
    }
    var doCheckerboard_0 : bool = (uboCS_0.enableCheckerboard_0) != u32(0);
    var doTemporalBlend_0 : bool = (uboCS_0.enableTemporalBlend_0) != u32(0);
    var doFullReproject_0 : bool = (uboCS_0.enableFullReproject_0) != u32(0);
    var _S25 : vec4<f32> = vec4<f32>(0.0f);
    var _S26 : f32 = uboCS_0.maxDepth_0;
    var isCheckerRender_0 : bool;
    if(doCheckerboard_0)
    {
        isCheckerRender_0 = true;
    }
    else
    {
        isCheckerRender_0 = doFullReproject_0;
    }
    var fragColor_0 : vec4<f32>;
    if(isCheckerRender_0)
    {
        var currentUV_0 : vec2<f32> = vec2<f32>(_S18.xy) / uboCS_0.resolution_0.xy;
        if(doCheckerboard_0)
        {
            isCheckerRender_0 = (((_S18.x + _S18.y) & (i32(1)))) == i32(((uboCS_0.frameCounter_0) & (u32(1))));
        }
        else
        {
            isCheckerRender_0 = true;
        }
        var reprojValid_0 : bool = false;
        var currUVPred_0 : vec2<f32> = currentUV_0;
        var reprojDepthVal_0 : f32 = _S26;
        var iter_0 : i32 = i32(0);
        var prevUV_0 : vec2<f32> = currentUV_0;
        for(;;)
        {
            if(iter_0 < i32(2))
            {
            }
            else
            {
                break;
            }
            var _S27 : f32 = prevUV_0.x;
            if(_S27 <= 0.00100000004749745f)
            {
                reproject1_0 = true;
            }
            else
            {
                reproject1_0 = _S27 >= 0.99900001287460327f;
            }
            if(reproject1_0)
            {
                reproject2_0 = true;
            }
            else
            {
                reproject2_0 = (prevUV_0.y) <= 0.00100000004749745f;
            }
            var _S28 : bool;
            if(reproject2_0)
            {
                _S28 = true;
            }
            else
            {
                _S28 = (prevUV_0.y) >= 0.99900001287460327f;
            }
            if(_S28)
            {
                reprojValid_0 = false;
                break;
            }
            var reprojDepthVal_1 : f32 = (textureSampleLevel((prevDepthTex_0), (prevDepthSampler_0), (prevUV_0), (0.0f))).x;
            if(reprojDepthVal_1 >= (uboCS_0.maxDepth_0 - 9.99999997475242708e-07f))
            {
                reprojValid_0 = false;
                reprojDepthVal_0 = reprojDepthVal_1;
                break;
            }
            var prevWorldPosH_0 : vec4<f32> = (((vec4<f32>(prevUV_0 * vec2<f32>(2.0f) - vec2<f32>(1.0f), reprojDepthVal_1 * 2.0f - 1.0f, 1.0f)) * (mat4x4<f32>(uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.prevInverseViewProjectionMatrix_0.data_0[i32(3)][i32(3)]))));
            var _S29 : f32 = prevWorldPosH_0.w;
            if((abs(_S29)) < 9.99999997475242708e-07f)
            {
                reprojValid_0 = false;
                reprojDepthVal_0 = reprojDepthVal_1;
                break;
            }
            var currClip_0 : vec4<f32> = (((vec4<f32>(prevWorldPosH_0.xyz / vec3<f32>(_S29), 1.0f)) * (mat4x4<f32>(uboCS_0.currentViewProjectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.currentViewProjectionMatrix_0.data_0[i32(3)][i32(3)]))));
            var _S30 : f32 = currClip_0.w;
            if(_S30 <= 0.0f)
            {
                reprojValid_0 = false;
                reprojDepthVal_0 = reprojDepthVal_1;
                break;
            }
            var _S31 : vec2<f32> = vec2<f32>(0.5f);
            var currUVPred_1 : vec2<f32> = currClip_0.xy / vec2<f32>(_S30) * _S31 + _S31;
            var prevUV_1 : vec2<f32> = prevUV_0 + (currentUV_0 - currUVPred_1);
            var iter_1 : i32 = iter_0 + i32(1);
            reprojValid_0 = true;
            currUVPred_0 = currUVPred_1;
            reprojDepthVal_0 = reprojDepthVal_1;
            iter_0 = iter_1;
            prevUV_0 = prevUV_1;
        }
        if(reprojValid_0)
        {
            reproject1_0 = (prevUV_0.x) > 0.00100000004749745f;
        }
        else
        {
            reproject1_0 = false;
        }
        if(reproject1_0)
        {
            reproject1_0 = (prevUV_0.x) < 0.99900001287460327f;
        }
        else
        {
            reproject1_0 = false;
        }
        if(reproject1_0)
        {
            reproject1_0 = (prevUV_0.y) > 0.00100000004749745f;
        }
        else
        {
            reproject1_0 = false;
        }
        if(reproject1_0)
        {
            reproject1_0 = (prevUV_0.y) < 0.99900001287460327f;
        }
        else
        {
            reproject1_0 = false;
        }
        if(reproject1_0)
        {
            var uvErr_0 : vec2<f32> = abs(currentUV_0 - currUVPred_0);
            var uvMaxErr_0 : vec2<f32> = vec2<f32>(2.0f) / uboCS_0.resolution_0.xy;
            if((uvErr_0.x) <= (uvMaxErr_0.x))
            {
                reprojValid_0 = (uvErr_0.y) <= (uvMaxErr_0.y);
            }
            else
            {
                reprojValid_0 = false;
            }
            if(reprojValid_0)
            {
                fragColor_0 = (textureSampleLevel((prevColorTex_0), (prevColorSampler_0), (prevUV_0), (0.0f)));
            }
            else
            {
                fragColor_0 = _S25;
            }
        }
        else
        {
            fragColor_0 = _S25;
        }
        if(doCheckerboard_0)
        {
            isCheckerRender_0 = !isCheckerRender_0;
        }
        else
        {
            isCheckerRender_0 = false;
        }
        if(isCheckerRender_0)
        {
            reproject1_0 = reprojValid_0;
        }
        else
        {
            reproject1_0 = false;
        }
        if(doFullReproject_0)
        {
            isCheckerRender_0 = (((uboCS_0.frameCounter_0) & (u32(1)))) == u32(1);
        }
        else
        {
            isCheckerRender_0 = false;
        }
        if(isCheckerRender_0)
        {
            reproject2_0 = reprojValid_0;
        }
        else
        {
            reproject2_0 = false;
        }
        if(reproject1_0)
        {
            isCheckerRender_0 = true;
        }
        else
        {
            isCheckerRender_0 = reproject2_0;
        }
        if(isCheckerRender_0)
        {
            var bloomColor_0 : vec4<f32> = vec4<f32>(GetSunColor_0(worldDir_0, 128.0f) * vec3<f32>(1.29999995231628418f), 1.0f);
            var cloudAlpha_0 : f32 = clamp(fragColor_0.w, 0.0f, 1.0f);
            var alphaness_0 : vec4<f32> = vec4<f32>(max(cloudAlpha_0, 0.20000000298023224f), 0.0f, 0.0f, 1.0f);
            if(cloudAlpha_0 > 0.10000000149011612f)
            {
                var _S32 : vec3<f32> = bloomColor_0.xyz * vec3<f32>((1.0f - cloudAlpha_0)) + mix(vec3<f32>(0.0f), bloomColor_0.xyz, vec3<f32>(clamp(ComputeFogAmount_0(startPos_4, 0.00002999999924214f), 0.0f, 1.0f))).xyz;
                bloomColor_0.x = _S32.x;
                bloomColor_0.y = _S32.y;
                bloomColor_0.z = _S32.z;
            }
            var _S33 : vec2<u32> = vec2<u32>(_S18);
            textureStore((outFragColor_0), (_S33), (fragColor_0));
            textureStore((outBloom_0), (_S33), (bloomColor_0));
            textureStore((outAlphaness_0), (_S33), (alphaness_0));
            textureStore((outCloudDistance_0), (_S33), (vec4<f32>(reprojDepthVal_0, 0.0f, 0.0f, 0.0f)));
            return;
        }
    }
    var _S34 : vec3<f32> = backgroundColor_0.xyz;
    var _S35 : vec2<u32> = vec2<u32>(_S18);
    var raymarchResult_0 : RaymarchToCloudResult_0 = RaymarchToCloud_0(startPos_4, endPos_1, _S34, _S35);
    var _S36 : vec3<f32> = raymarchResult_0.color_0.xyz * vec3<f32>(1.79999995231628418f) - vec3<f32>((0.10000000149011612f * raymarchResult_0.color_0.w));
    raymarchResult_0.color_0.x = _S36.x;
    raymarchResult_0.color_0.y = _S36.y;
    raymarchResult_0.color_0.z = _S36.z;
    var _S37 : vec3<f32> = mix(raymarchResult_0.color_0.xyz, _S34 * vec3<f32>(raymarchResult_0.color_0.w), vec3<f32>(clamp(fogAmount_1, 0.0f, 1.0f)));
    raymarchResult_0.color_0.x = _S37.x;
    raymarchResult_0.color_0.y = _S37.y;
    raymarchResult_0.color_0.z = _S37.z;
    var _S38 : vec3<f32> = raymarchResult_0.color_0.xyz + vec3<f32>(0.80000001192092896f) * vec3<f32>(1.0f, 0.40000000596046448f, 0.20000000298023224f) * vec3<f32>(pow(clamp(dot(uboCS_0.lightDirection_0.xyz, normalize(endPos_1 - startPos_4)), 0.0f, 1.0f), 256.0f)) * vec3<f32>(raymarchResult_0.color_0.w);
    raymarchResult_0.color_0.x = _S38.x;
    raymarchResult_0.color_0.y = _S38.y;
    raymarchResult_0.color_0.z = _S38.z;
    var bloomColor_1 : vec4<f32> = vec4<f32>(GetSunColor_0(worldDir_0, 128.0f) * vec3<f32>(1.29999995231628418f), 1.0f);
    var cloudAlphaness_0 : f32 = clamp(raymarchResult_0.color_0.w, 0.0f, 1.0f);
    var alphaness_1 : vec4<f32> = vec4<f32>(max(cloudAlphaness_0, 0.20000000298023224f), 0.0f, 0.0f, 1.0f);
    if(cloudAlphaness_0 > 0.10000000149011612f)
    {
        var _S39 : vec3<f32> = bloomColor_1.xyz * vec3<f32>((1.0f - cloudAlphaness_0)) + mix(vec3<f32>(0.0f), bloomColor_1.xyz, vec3<f32>(clamp(ComputeFogAmount_0(startPos_4, 0.00002999999924214f), 0.0f, 1.0f))).xyz;
        bloomColor_1.x = _S39.x;
        bloomColor_1.y = _S39.y;
        bloomColor_1.z = _S39.z;
    }
    var fragColor_1 : vec4<f32> = vec4<f32>(raymarchResult_0.color_0.xyz, raymarchResult_0.color_0.w);
    if(doTemporalBlend_0)
    {
        isCheckerRender_0 = raymarchResult_0.hit_0;
    }
    else
    {
        isCheckerRender_0 = false;
    }
    if(isCheckerRender_0)
    {
        var prevClip_0 : vec4<f32> = (((vec4<f32>(raymarchResult_0.position_0, 1.0f)) * (mat4x4<f32>(uboCS_0.prevViewProjectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.prevViewProjectionMatrix_0.data_0[i32(3)][i32(3)]))));
        var _S40 : f32 = prevClip_0.w;
        if(_S40 > 0.0f)
        {
            var _S41 : vec2<f32> = vec2<f32>(0.5f);
            var prevUV_2 : vec2<f32> = prevClip_0.xy / vec2<f32>(_S40) * _S41 + _S41;
            var _S42 : f32 = prevUV_2.x;
            if(_S42 > 0.00100000004749745f)
            {
                isCheckerRender_0 = _S42 < 0.99900001287460327f;
            }
            else
            {
                isCheckerRender_0 = false;
            }
            if(isCheckerRender_0)
            {
                isCheckerRender_0 = (prevUV_2.y) > 0.00100000004749745f;
            }
            else
            {
                isCheckerRender_0 = false;
            }
            if(isCheckerRender_0)
            {
                isCheckerRender_0 = (prevUV_2.y) < 0.99900001287460327f;
            }
            else
            {
                isCheckerRender_0 = false;
            }
            if(isCheckerRender_0)
            {
                fragColor_0 = mix((textureSampleLevel((prevColorTex_0), (prevColorSampler_0), (prevUV_2), (0.0f))), fragColor_1, vec4<f32>(uboCS_0.reprojectionBlend_0));
            }
            else
            {
                fragColor_0 = fragColor_1;
            }
        }
        else
        {
            fragColor_0 = fragColor_1;
        }
    }
    else
    {
        fragColor_0 = fragColor_1;
    }
    var depth_0 : vec4<f32> = vec4<f32>(ComputeDepth_0(mat4x4<f32>(uboCS_0.projectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.projectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.projectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.projectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.projectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.projectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.projectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.projectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.projectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.projectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.projectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.projectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.projectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.projectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.projectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.projectionMatrix_0.data_0[i32(3)][i32(3)]), mat4x4<f32>(uboCS_0.viewMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.viewMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.viewMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.viewMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.viewMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.viewMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.viewMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.viewMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.viewMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.viewMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.viewMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.viewMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.viewMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.viewMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.viewMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.viewMatrix_0.data_0[i32(3)][i32(3)]), raymarchResult_0.position_0), 0.0f, 0.0f, 0.0f);
    textureStore((outFragColor_0), (_S35), (fragColor_0));
    textureStore((outBloom_0), (_S35), (bloomColor_1));
    textureStore((outAlphaness_0), (_S35), (alphaness_1));
    textureStore((outCloudDistance_0), (_S35), (depth_0));
    return;
}

