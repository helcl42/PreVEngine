struct SkyPostProcessParams_std140_0
{
    @align(16) resolution_0 : vec4<f32>,
    @align(16) lightPosition_0 : vec4<f32>,
    @align(16) enableGodRays_0 : u32,
    @align(4) lightDotCameraForward_0 : f32,
};

@binding(5) @group(0) var<uniform> uboCS_0 : SkyPostProcessParams_std140_0;
@binding(1) @group(0) var skyTex_0 : texture_2d<f32>;

@binding(2) @group(0) var skySampler_0 : sampler;

@binding(3) @group(0) var bloomTex_0 : texture_2d<f32>;

@binding(4) @group(0) var bloomSampler_0 : sampler;

@binding(0) @group(0) var outFragColor_0 : texture_storage_2d<rgba8unorm, write>;

fn GaussianBlur_0( tex_0 : texture_2d<f32>,  texSampler_0 : sampler,  uv_0 : vec2<f32>) -> vec4<f32>
{
    var BLUR_OFFSET_X_0 : f32 = 1.0f / uboCS_0.resolution_0.x;
    var BLUR_OFFSET_Y_0 : f32 = 1.0f / uboCS_0.resolution_0.y;
    var _S1 : vec2<f32> = vec2<f32>(BLUR_OFFSET_X_0, 0.0f);
    var _S2 : f32 = - BLUR_OFFSET_Y_0;
    var _S3 : vec2<f32> = vec2<f32>(BLUR_OFFSET_X_0, _S2);
    var _S4 : array<vec2<f32>, i32(9)> = array<vec2<f32>, i32(9)>( vec2<f32>(- BLUR_OFFSET_X_0, BLUR_OFFSET_Y_0), vec2<f32>(0.0f, BLUR_OFFSET_Y_0), vec2<f32>(BLUR_OFFSET_X_0, BLUR_OFFSET_Y_0), _S1, vec2<f32>(0.0f, 0.0f), _S1, _S3, vec2<f32>(0.0f, _S2), _S3 );
    var _S5 : array<f32, i32(9)> = array<f32, i32(9)>( 0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f );
    var sampleTex_0 : array<vec4<f32>, i32(9)>;
    var i_0 : i32 = i32(0);
    for(;;)
    {
        if(i_0 < i32(9))
        {
        }
        else
        {
            break;
        }
        sampleTex_0[i_0] = (textureSampleLevel((tex_0), (texSampler_0), (uv_0 + _S4[i_0]), (0.0f)));
        i_0 = i_0 + i32(1);
    }
    var _S6 : vec4<f32> = vec4<f32>(0.0f);
    var j_0 : i32 = i32(0);
    var col_0 : vec4<f32> = _S6;
    for(;;)
    {
        if(j_0 < i32(9))
        {
        }
        else
        {
            break;
        }
        var col_1 : vec4<f32> = col_0 + sampleTex_0[j_0] * vec4<f32>(_S5[j_0]);
        j_0 = j_0 + i32(1);
        col_0 = col_1;
    }
    return col_0;
}

@compute
@workgroup_size(16, 16, 1)
fn computeMain(@builtin(global_invocation_id) dispatchThreadID_0 : vec3<u32>)
{
    var _S7 : vec2<i32> = vec2<i32>(dispatchThreadID_0.xy);
    var uv_1 : vec2<f32> = vec2<f32>(_S7.xy) / uboCS_0.resolution_0.xy;
    var fragColor_0 : vec4<f32> = GaussianBlur_0(skyTex_0, skySampler_0, uv_1);
    var _S8 : bool;
    if((uboCS_0.lightDotCameraForward_0) > 0.0f)
    {
        _S8 = (uboCS_0.enableGodRays_0) != u32(0);
    }
    else
    {
        _S8 = false;
    }
    if(_S8)
    {
        var textureCoordDelta_0 : vec2<f32> = (uv_1 - uboCS_0.lightPosition_0.xy) * vec2<f32>(0.01406249962747097f);
        var _S9 : vec3<f32> = GaussianBlur_0(bloomTex_0, bloomSampler_0, uv_1).xyz * vec3<f32>(0.40000000596046448f);
        var i_1 : i32 = i32(0);
        var sampleUv_0 : vec2<f32> = uv_1;
        var illuminationDecay_0 : f32 = 1.0f;
        var colRays_0 : vec3<f32> = _S9;
        for(;;)
        {
            if(i_1 < i32(64))
            {
            }
            else
            {
                break;
            }
            var sampleUv_1 : vec2<f32> = sampleUv_0 - textureCoordDelta_0;
            var colRays_1 : vec3<f32> = colRays_0 + (textureSampleLevel((bloomTex_0), (bloomSampler_0), (sampleUv_1), (0.0f))).xyz * vec3<f32>(illuminationDecay_0) * vec3<f32>(0.07000000029802322f);
            var illuminationDecay_1 : f32 = illuminationDecay_0 * 0.98000001907348633f;
            i_1 = i_1 + i32(1);
            sampleUv_0 = sampleUv_1;
            illuminationDecay_0 = illuminationDecay_1;
            colRays_0 = colRays_1;
        }
        var _S10 : vec3<f32> = mix(fragColor_0.xyz, (fragColor_0.xyz + smoothstep(vec3<f32>(0.0f), vec3<f32>(1.0f), colRays_0) * vec3<f32>(0.44999998807907104f)) * vec3<f32>(0.89999997615814209f), vec3<f32>((uboCS_0.lightDotCameraForward_0 * uboCS_0.lightDotCameraForward_0)));
        fragColor_0.x = _S10.x;
        fragColor_0.y = _S10.y;
        fragColor_0.z = _S10.z;
    }
    textureStore((outFragColor_0), (vec2<u32>(_S7)), (fragColor_0));
    return;
}

