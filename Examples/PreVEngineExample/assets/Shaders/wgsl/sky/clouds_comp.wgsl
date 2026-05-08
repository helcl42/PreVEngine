struct UniformBufferObject {
    outputTextureSize: vec4<f32>,
    seed: vec4<f32>,
    perlinAmplitude: f32,
    perlinFrequency: f32,
    perlinScale: f32,
    perlinOctaves: i32,
}

@group(0) @binding(0) 
var<uniform> uboCS: UniformBufferObject;
var<private> gl_GlobalInvocationID_1: vec3<u32>;
@group(0) @binding(1) 
var outWeatherTexture: texture_storage_2d<rgba8unorm,write>;

fn Random2D_u0028_vf2_u003b(st: ptr<function, vec2<f32>>) -> f32 {
    let _e31 = (*st);
    let _e33 = uboCS.seed;
    return fract((sin(dot(_e31, (vec2<f32>(12.9898f, 78.233f) + _e33.xy))) * 43758.547f));
}

fn NoiseInterpolation_u0028_vf2_u003b_f1_u003b(iCoord: ptr<function, vec2<f32>>, iSize: ptr<function, f32>) -> f32 {
    var grid: vec2<f32>;
    var randomInput: vec2<f32>;
    var p0_: f32;
    var param: vec2<f32>;
    var p1_: f32;
    var param_1: vec2<f32>;
    var p2_: f32;
    var param_2: vec2<f32>;
    var p3_: f32;
    var param_3: vec2<f32>;
    var gridFraction: vec2<f32>;
    var weights: vec2<f32>;

    let _e44 = (*iCoord);
    let _e45 = (*iSize);
    grid = (_e44 * _e45);
    let _e47 = grid;
    randomInput = floor(_e47);
    let _e49 = randomInput;
    param = _e49;
    let _e50 = Random2D_u0028_vf2_u003b((&param));
    p0_ = _e50;
    let _e51 = randomInput;
    param_1 = (_e51 + vec2<f32>(1f, 0f));
    let _e53 = Random2D_u0028_vf2_u003b((&param_1));
    p1_ = _e53;
    let _e54 = randomInput;
    param_2 = (_e54 + vec2<f32>(0f, 1f));
    let _e56 = Random2D_u0028_vf2_u003b((&param_2));
    p2_ = _e56;
    let _e57 = randomInput;
    param_3 = (_e57 + vec2<f32>(1f, 1f));
    let _e59 = Random2D_u0028_vf2_u003b((&param_3));
    p3_ = _e59;
    let _e60 = grid;
    gridFraction = fract(_e60);
    let _e62 = gridFraction;
    weights = smoothstep(vec2<f32>(0f, 0f), vec2<f32>(1f, 1f), _e62);
    let _e64 = p0_;
    let _e65 = p1_;
    let _e66 = p0_;
    let _e69 = weights[0u];
    let _e72 = p2_;
    let _e73 = p0_;
    let _e76 = weights[1u];
    let _e79 = weights[0u];
    let _e83 = p3_;
    let _e84 = p1_;
    let _e87 = weights[1u];
    let _e89 = weights[0u];
    return (((_e64 + ((_e65 - _e66) * _e69)) + (((_e72 - _e73) * _e76) * (1f - _e79))) + ((_e83 - _e84) * (_e87 * _e89)));
}

fn PerlinNoise_u0028_vf2_u003b_f1_u003b_f1_u003b_f1_u003b_i1_u003b(uv: ptr<function, vec2<f32>>, sc: ptr<function, f32>, f: ptr<function, f32>, a: ptr<function, f32>, o: ptr<function, i32>) -> f32 {
    var noiseValue: f32;
    var localAmplitude: f32;
    var localFrequency: f32;
    var index: i32;
    var param_4: vec2<f32>;
    var param_5: f32;

    noiseValue = 0f;
    let _e41 = (*a);
    localAmplitude = _e41;
    let _e42 = (*f);
    localFrequency = _e42;
    index = 0i;
    loop {
        let _e43 = index;
        let _e44 = (*o);
        if (_e43 < _e44) {
            let _e46 = (*sc);
            let _e47 = localFrequency;
            let _e49 = (*uv);
            param_4 = _e49;
            param_5 = (_e46 * _e47);
            let _e50 = NoiseInterpolation_u0028_vf2_u003b_f1_u003b((&param_4), (&param_5));
            let _e51 = localAmplitude;
            let _e53 = noiseValue;
            noiseValue = (_e53 + (_e50 * _e51));
            let _e55 = localAmplitude;
            localAmplitude = (_e55 * 0.25f);
            let _e57 = localFrequency;
            localFrequency = (_e57 * 3f);
            continue;
        } else {
            break;
        }
        continuing {
            let _e59 = index;
            index = (_e59 + 1i);
        }
    }
    let _e61 = noiseValue;
    let _e62 = noiseValue;
    return (_e61 * _e62);
}

fn main_1() {
    var pixel: vec2<i32>;
    var uv_1: vec2<f32>;
    var suv: vec2<f32>;
    var cloudType: f32;
    var param_6: vec2<f32>;
    var param_7: f32;
    var param_8: f32;
    var param_9: f32;
    var param_10: i32;
    var coverage: f32;
    var param_11: vec2<f32>;
    var param_12: f32;
    var param_13: f32;
    var param_14: f32;
    var param_15: i32;
    var weather: vec4<f32>;

    let _e46 = gl_GlobalInvocationID_1;
    pixel = bitcast<vec2<i32>>(_e46.xy);
    let _e49 = pixel;
    let _e52 = uboCS.outputTextureSize;
    uv_1 = (vec2<f32>(_e49) / _e52.xy);
    let _e55 = uv_1;
    suv = (_e55 + vec2<f32>(5.5f, 5.5f));
    let _e58 = uboCS.perlinScale;
    let _e60 = suv;
    param_6 = _e60;
    param_7 = (_e58 * 3f);
    param_8 = 0.3f;
    param_9 = 0.7f;
    param_10 = 10i;
    let _e61 = PerlinNoise_u0028_vf2_u003b_f1_u003b_f1_u003b_f1_u003b_i1_u003b((&param_6), (&param_7), (&param_8), (&param_9), (&param_10));
    cloudType = clamp(_e61, 0f, 1f);
    let _e63 = uv_1;
    param_11 = _e63;
    let _e65 = uboCS.perlinScale;
    param_12 = _e65;
    let _e67 = uboCS.perlinFrequency;
    param_13 = _e67;
    let _e69 = uboCS.perlinAmplitude;
    param_14 = _e69;
    let _e71 = uboCS.perlinOctaves;
    param_15 = _e71;
    let _e72 = PerlinNoise_u0028_vf2_u003b_f1_u003b_f1_u003b_f1_u003b_i1_u003b((&param_11), (&param_12), (&param_13), (&param_14), (&param_15));
    coverage = _e72;
    let _e73 = coverage;
    let _e75 = cloudType;
    weather = vec4<f32>(clamp(_e73, 0f, 1f), _e75, 0f, 1f);
    let _e77 = pixel;
    let _e78 = weather;
    textureStore(outWeatherTexture, _e77, _e78);
    return;
}

@compute @workgroup_size(16, 16, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
