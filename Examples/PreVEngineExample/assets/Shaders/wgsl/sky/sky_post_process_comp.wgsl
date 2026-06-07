struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct SkyPostProcessParams_std140_0
{
    @align(16) resolution_0 : vec4<f32>,
    @align(16) lightPosition_0 : vec4<f32>,
    @align(16) inverseProjectionMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) inverseViewMatrix_0 : _MatrixStorage_float4x4_ColMajorstd140_0,
    @align(16) lightDirection_0 : vec4<f32>,
    @align(16) skyColorBottom_0 : vec4<f32>,
    @align(16) skyColorTop_0 : vec4<f32>,
    @align(16) enableGodRays_0 : u32,
    @align(4) enableBlur_0 : u32,
    @align(8) lightDotCameraForward_0 : f32,
    @align(4) godRaySamples_0 : u32,
    @align(16) enableCheckerboardResolve_0 : u32,
    @align(4) frameCounter_0 : u32,
    @align(8) pad0_0 : f32,
    @align(4) pad1_0 : f32,
};

@binding(5) @group(0) var<uniform> uboCS_0 : SkyPostProcessParams_std140_0;
@binding(1) @group(0) var skyTex_0 : texture_2d<f32>;

@binding(2) @group(0) var skySampler_0 : sampler;

@binding(3) @group(0) var bloomTex_0 : texture_2d<f32>;

@binding(4) @group(0) var bloomSampler_0 : sampler;

@binding(0) @group(0) var outFragColor_0 : texture_storage_2d<rgba8unorm, write>;

fn GetWorldDirection_0( uv_0 : vec2<f32>) -> vec3<f32>
{
    return normalize((((vec4<f32>((((vec4<f32>(uv_0 * vec2<f32>(2.0f) - vec2<f32>(1.0f), 1.0f, 1.0f)) * (mat4x4<f32>(uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.inverseProjectionMatrix_0.data_0[i32(3)][i32(3)])))).xy, -1.0f, 0.0f)) * (mat4x4<f32>(uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(0)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(1)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(2)], uboCS_0.inverseViewMatrix_0.data_0[i32(0)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(1)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(2)][i32(3)], uboCS_0.inverseViewMatrix_0.data_0[i32(3)][i32(3)])))).xyz);
}

fn GetSunColor_0( worldRayDirection_0 : vec3<f32>,  powExp_0 : f32) -> vec3<f32>
{
    return vec3<f32>(0.80000001192092896f) * vec3<f32>(1.0f, 0.60000002384185791f, 0.10000000149011612f) * vec3<f32>(pow(clamp(dot(uboCS_0.lightDirection_0.xyz, worldRayDirection_0), 0.0f, 1.0f), powExp_0));
}

fn GetBackgroundColor_0( worldRayDirection_1 : vec3<f32>) -> vec4<f32>
{
    return vec4<f32>(mix(uboCS_0.skyColorBottom_0.xyz, uboCS_0.skyColorTop_0.xyz, vec3<f32>(smoothstep(-0.07000000029802322f, 0.07000000029802322f, worldRayDirection_1.y))) + GetSunColor_0(worldRayDirection_1, 350.0f), 1.0f);
}

fn CompositeAndSample_0( tex_0 : texture_2d<f32>,  texSampler_0 : sampler,  uv_1 : vec2<f32>) -> vec4<f32>
{
    var cloudData_0 : vec4<f32> = (textureSampleLevel((tex_0), (texSampler_0), (uv_1), (0.0f)));
    return vec4<f32>(GetBackgroundColor_0(GetWorldDirection_0(uv_1)).xyz * vec3<f32>((1.0f - cloudData_0.w)) + cloudData_0.xyz, 1.0f);
}

fn GaussianBlurComposited_0( tex_1 : texture_2d<f32>,  texSampler_1 : sampler,  uv_2 : vec2<f32>) -> vec4<f32>
{
    var BLUR_OFFSET_X_0 : f32 = 1.0f / uboCS_0.resolution_0.x;
    var BLUR_OFFSET_Y_0 : f32 = 1.0f / uboCS_0.resolution_0.y;
    var _S1 : f32 = - BLUR_OFFSET_X_0;
    var _S2 : f32 = - BLUR_OFFSET_Y_0;
    var _S3 : array<vec2<f32>, i32(9)> = array<vec2<f32>, i32(9)>( vec2<f32>(_S1, BLUR_OFFSET_Y_0), vec2<f32>(0.0f, BLUR_OFFSET_Y_0), vec2<f32>(BLUR_OFFSET_X_0, BLUR_OFFSET_Y_0), vec2<f32>(_S1, 0.0f), vec2<f32>(0.0f, 0.0f), vec2<f32>(BLUR_OFFSET_X_0, 0.0f), vec2<f32>(_S1, _S2), vec2<f32>(0.0f, _S2), vec2<f32>(BLUR_OFFSET_X_0, _S2) );
    var _S4 : array<f32, i32(9)> = array<f32, i32(9)>( 0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f );
    var _S5 : vec4<f32> = vec4<f32>(0.0f);
    var i_0 : i32 = i32(0);
    var col_0 : vec4<f32> = _S5;
    for(;;)
    {
        if(i_0 < i32(9))
        {
        }
        else
        {
            break;
        }
        var col_1 : vec4<f32> = col_0 + CompositeAndSample_0(tex_1, texSampler_1, uv_2 + _S3[i_0]) * vec4<f32>(_S4[i_0]);
        i_0 = i_0 + i32(1);
        col_0 = col_1;
    }
    return col_0;
}

fn CheckerboardResolve_0( tex_2 : texture_2d<f32>,  texSampler_2 : sampler,  fragCoord_0 : vec2<i32>,  uv_3 : vec2<f32>) -> vec4<f32>
{
    if((((fragCoord_0.x + fragCoord_0.y) & (i32(1)))) != i32(((uboCS_0.frameCounter_0) & (u32(1)))))
    {
        var texelSize_0 : vec2<f32> = vec2<f32>(1.0f) / uboCS_0.resolution_0.xy;
        var _S6 : f32 = texelSize_0.x;
        var _S7 : f32 = - _S6;
        var _S8 : f32 = texelSize_0.y;
        var _S9 : f32 = - _S8;
        return mix(CompositeAndSample_0(tex_2, texSampler_2, uv_3), (CompositeAndSample_0(tex_2, texSampler_2, uv_3 + vec2<f32>(_S7, _S9)) + CompositeAndSample_0(tex_2, texSampler_2, uv_3 + vec2<f32>(_S6, _S9)) + CompositeAndSample_0(tex_2, texSampler_2, uv_3 + vec2<f32>(_S7, _S8)) + CompositeAndSample_0(tex_2, texSampler_2, uv_3 + vec2<f32>(_S6, _S8))) * vec4<f32>(0.25f), vec4<f32>(0.5f));
    }
    return CompositeAndSample_0(tex_2, texSampler_2, uv_3);
}

fn GaussianBlur_0( tex_3 : texture_2d<f32>,  texSampler_3 : sampler,  uv_4 : vec2<f32>) -> vec4<f32>
{
    var BLUR_OFFSET_X_1 : f32 = 1.0f / uboCS_0.resolution_0.x;
    var BLUR_OFFSET_Y_1 : f32 = 1.0f / uboCS_0.resolution_0.y;
    var _S10 : f32 = - BLUR_OFFSET_X_1;
    var _S11 : f32 = - BLUR_OFFSET_Y_1;
    var _S12 : array<vec2<f32>, i32(9)> = array<vec2<f32>, i32(9)>( vec2<f32>(_S10, BLUR_OFFSET_Y_1), vec2<f32>(0.0f, BLUR_OFFSET_Y_1), vec2<f32>(BLUR_OFFSET_X_1, BLUR_OFFSET_Y_1), vec2<f32>(_S10, 0.0f), vec2<f32>(0.0f, 0.0f), vec2<f32>(BLUR_OFFSET_X_1, 0.0f), vec2<f32>(_S10, _S11), vec2<f32>(0.0f, _S11), vec2<f32>(BLUR_OFFSET_X_1, _S11) );
    var _S13 : array<f32, i32(9)> = array<f32, i32(9)>( 0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f );
    var _S14 : vec4<f32> = vec4<f32>(0.0f);
    var i_1 : i32 = i32(0);
    var col_2 : vec4<f32> = _S14;
    for(;;)
    {
        if(i_1 < i32(9))
        {
        }
        else
        {
            break;
        }
        var col_3 : vec4<f32> = col_2 + (textureSampleLevel((tex_3), (texSampler_3), (uv_4 + _S12[i_1]), (0.0f))) * vec4<f32>(_S13[i_1]);
        i_1 = i_1 + i32(1);
        col_2 = col_3;
    }
    return col_2;
}

@compute
@workgroup_size(16, 16, 1)
fn computeMain(@builtin(global_invocation_id) dispatchThreadID_0 : vec3<u32>)
{
    var _S15 : vec2<i32> = vec2<i32>(dispatchThreadID_0.xy);
    var uv_5 : vec2<f32> = vec2<f32>(_S15.xy) / uboCS_0.resolution_0.xy;
    var fragColor_0 : vec4<f32>;
    if((uboCS_0.enableBlur_0) != u32(0))
    {
        fragColor_0 = GaussianBlurComposited_0(skyTex_0, skySampler_0, uv_5);
    }
    else
    {
        if((uboCS_0.enableCheckerboardResolve_0) != u32(0))
        {
            fragColor_0 = CheckerboardResolve_0(skyTex_0, skySampler_0, _S15, uv_5);
        }
        else
        {
            fragColor_0 = CompositeAndSample_0(skyTex_0, skySampler_0, uv_5);
        }
    }
    var _S16 : bool;
    if((uboCS_0.lightDotCameraForward_0) > 0.0f)
    {
        _S16 = (uboCS_0.enableGodRays_0) != u32(0);
    }
    else
    {
        _S16 = false;
    }
    if(_S16)
    {
        var SAMPLES_0 : i32 = i32(uboCS_0.godRaySamples_0);
        var textureCoordDelta_0 : vec2<f32> = (uv_5 - uboCS_0.lightPosition_0.xy) * vec2<f32>((0.89999997615814209f / f32(SAMPLES_0)));
        var _S17 : vec3<f32> = GaussianBlur_0(bloomTex_0, bloomSampler_0, uv_5).xyz * vec3<f32>(0.40000000596046448f);
        var i_2 : i32 = i32(0);
        var sampleUv_0 : vec2<f32> = uv_5;
        var illuminationDecay_0 : f32 = 1.0f;
        var colRays_0 : vec3<f32> = _S17;
        for(;;)
        {
            if(i_2 < SAMPLES_0)
            {
            }
            else
            {
                break;
            }
            var sampleUv_1 : vec2<f32> = sampleUv_0 - textureCoordDelta_0;
            var colRays_1 : vec3<f32> = colRays_0 + (textureSampleLevel((bloomTex_0), (bloomSampler_0), (sampleUv_1), (0.0f))).xyz * vec3<f32>(illuminationDecay_0) * vec3<f32>(0.07000000029802322f);
            var illuminationDecay_1 : f32 = illuminationDecay_0 * 0.98000001907348633f;
            i_2 = i_2 + i32(1);
            sampleUv_0 = sampleUv_1;
            illuminationDecay_0 = illuminationDecay_1;
            colRays_0 = colRays_1;
        }
        var _S18 : vec3<f32> = mix(fragColor_0.xyz, (fragColor_0.xyz + smoothstep(vec3<f32>(0.0f), vec3<f32>(1.0f), colRays_0) * vec3<f32>(0.44999998807907104f)) * vec3<f32>(0.89999997615814209f), vec3<f32>((uboCS_0.lightDotCameraForward_0 * uboCS_0.lightDotCameraForward_0)));
        fragColor_0.x = _S18.x;
        fragColor_0.y = _S18.y;
        fragColor_0.z = _S18.z;
    }
    textureStore((outFragColor_0), (vec2<u32>(_S15)), (fragColor_0));
    return;
}

