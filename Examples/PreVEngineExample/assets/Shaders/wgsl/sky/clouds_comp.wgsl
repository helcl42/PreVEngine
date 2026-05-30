struct CloudParams_std140_0
{
    @align(16) outputTextureSize_0 : vec4<f32>,
    @align(16) seed_0 : vec4<f32>,
    @align(16) perlinAmplitude_0 : f32,
    @align(4) perlinFrequency_0 : f32,
    @align(8) perlinScale_0 : f32,
    @align(4) perlinOctaves_0 : i32,
};

@binding(0) @group(0) var<uniform> uboCS_0 : CloudParams_std140_0;
@binding(1) @group(0) var outWeatherTexture_0 : texture_storage_2d<rgba8unorm, write>;

fn Random2D_0( st_0 : vec2<f32>) -> f32
{
    return fract(sin(dot(st_0.xy, vec2<f32>(12.98980045318603516f, 78.233001708984375f) + uboCS_0.seed_0.xy)) * 43758.546875f);
}

fn NoiseInterpolation_0( iCoord_0 : vec2<f32>,  iSize_0 : f32) -> f32
{
    var grid_0 : vec2<f32> = iCoord_0 * vec2<f32>(iSize_0);
    var randomInput_0 : vec2<f32> = floor(grid_0);
    var p0_0 : f32 = Random2D_0(randomInput_0);
    var p1_0 : f32 = Random2D_0(randomInput_0 + vec2<f32>(1.0f, 0.0f));
    const _S1 : vec2<f32> = vec2<f32>(1.0f, 1.0f);
    var weights_0 : vec2<f32> = smoothstep(vec2<f32>(0.0f, 0.0f), _S1, fract(grid_0));
    var _S2 : f32 = weights_0.x;
    var _S3 : f32 = weights_0.y;
    return p0_0 + (p1_0 - p0_0) * _S2 + (Random2D_0(randomInput_0 + vec2<f32>(0.0f, 1.0f)) - p0_0) * _S3 * (1.0f - _S2) + (Random2D_0(randomInput_0 + _S1) - p1_0) * (_S3 * _S2);
}

fn PerlinNoise_0( uv_0 : vec2<f32>,  sc_0 : f32,  f_0 : f32,  a_0 : f32,  o_0 : i32) -> f32
{
    var index_0 : i32 = i32(0);
    var localFrequency_0 : f32 = f_0;
    var localAmplitude_0 : f32 = a_0;
    var noiseValue_0 : f32 = 0.0f;
    for(;;)
    {
        if(index_0 < o_0)
        {
        }
        else
        {
            break;
        }
        var noiseValue_1 : f32 = noiseValue_0 + NoiseInterpolation_0(uv_0, sc_0 * localFrequency_0) * localAmplitude_0;
        var localAmplitude_1 : f32 = localAmplitude_0 * 0.25f;
        var localFrequency_1 : f32 = localFrequency_0 * 3.0f;
        index_0 = index_0 + i32(1);
        localFrequency_0 = localFrequency_1;
        localAmplitude_0 = localAmplitude_1;
        noiseValue_0 = noiseValue_1;
    }
    return noiseValue_0 * noiseValue_0;
}

@compute
@workgroup_size(16, 16, 1)
fn computeMain(@builtin(global_invocation_id) dispatchThreadID_0 : vec3<u32>)
{
    var _S4 : vec2<i32> = vec2<i32>(dispatchThreadID_0.xy);
    var uv_1 : vec2<f32> = vec2<f32>(_S4) / uboCS_0.outputTextureSize_0.xy;
    textureStore((outWeatherTexture_0), (vec2<u32>(_S4)), (vec4<f32>(clamp(PerlinNoise_0(uv_1, uboCS_0.perlinScale_0, uboCS_0.perlinFrequency_0, uboCS_0.perlinAmplitude_0, uboCS_0.perlinOctaves_0), 0.0f, 1.0f), clamp(PerlinNoise_0(uv_1 + vec2<f32>(5.5f, 5.5f), uboCS_0.perlinScale_0 * 3.0f, 0.30000001192092896f, 0.69999998807907104f, i32(10)), 0.0f, 1.0f), 0.0f, 1.0f)));
    return;
}

