struct UniformBufferObject {
    resolution: vec4<f32>,
    lightPosition: vec4<f32>,
    enableGodRays: u32,
    lightDotCameraForward: f32,
}

@group(0) @binding(5) 
var<uniform> uboCS: UniformBufferObject;
var<private> gl_GlobalInvocationID_1: vec3<u32>;
@group(0) @binding(1) 
var skyTex: texture_2d<f32>;
@group(0) @binding(2) 
var skySampler: sampler;
@group(0) @binding(3) 
var bloomTex: texture_2d<f32>;
@group(0) @binding(4) 
var bloomSampler: sampler;
@group(0) @binding(0) 
var outFragColor: texture_storage_2d<rgba8unorm,write>;

fn GaussianBlur_u0028_t21_u003b_p1_u003b_vf2_u003b(tex: texture_2d<f32>, texSampler: sampler, uv: ptr<function, vec2<f32>>) -> vec4<f32> {
    var offsets: array<vec2<f32>, 9>;
    var kernel: array<f32, 9>;
    var i: i32;
    var pixel: vec4<f32>;
    var sampleTex: array<vec4<f32>, 9>;
    var col: vec4<f32>;
    var i_1: i32;

    let _e45 = uboCS.resolution[0u];
    let _e50 = uboCS.resolution[1u];
    let _e55 = uboCS.resolution[1u];
    let _e60 = uboCS.resolution[0u];
    let _e64 = uboCS.resolution[1u];
    let _e69 = uboCS.resolution[0u];
    let _e74 = uboCS.resolution[0u];
    let _e79 = uboCS.resolution[0u];
    let _e83 = uboCS.resolution[1u];
    let _e89 = uboCS.resolution[1u];
    let _e95 = uboCS.resolution[0u];
    let _e99 = uboCS.resolution[1u];
    offsets = array<vec2<f32>, 9>(vec2<f32>(-((1f / _e45)), (1f / _e50)), vec2<f32>(0f, (1f / _e55)), vec2<f32>((1f / _e60), (1f / _e64)), vec2<f32>((1f / _e69), 0f), vec2<f32>(0f, 0f), vec2<f32>((1f / _e74), 0f), vec2<f32>((1f / _e79), -((1f / _e83))), vec2<f32>(0f, -((1f / _e89))), vec2<f32>((1f / _e95), -((1f / _e99))));
    kernel = array<f32, 9>(0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);
    i = 0i;
    loop {
        let _e104 = i;
        if (_e104 < 9i) {
            let _e106 = (*uv);
            let _e107 = i;
            let _e109 = offsets[_e107];
            let _e111 = textureSampleLevel(tex, texSampler, (_e106 + _e109), 0f);
            pixel = _e111;
            let _e112 = i;
            let _e113 = pixel;
            sampleTex[_e112] = _e113;
            continue;
        } else {
            break;
        }
        continuing {
            let _e115 = i;
            i = (_e115 + 1i);
        }
    }
    col = vec4<f32>(0f, 0f, 0f, 0f);
    i_1 = 0i;
    loop {
        let _e117 = i_1;
        if (_e117 < 9i) {
            let _e119 = i_1;
            let _e121 = sampleTex[_e119];
            let _e122 = i_1;
            let _e124 = kernel[_e122];
            let _e126 = col;
            col = (_e126 + (_e121 * _e124));
            continue;
        } else {
            break;
        }
        continuing {
            let _e128 = i_1;
            i_1 = (_e128 + 1i);
        }
    }
    let _e130 = col;
    return _e130;
}

fn main_1() {
    var fragCoord: vec2<i32>;
    var uv_1: vec2<f32>;
    var fragColor: vec4<f32>;
    var param: vec2<f32>;
    var decay: f32;
    var density: f32;
    var weight: f32;
    var exposure: f32;
    var illuminationDecay: f32;
    var textureCoord: vec2<f32>;
    var textureCoordDelta: vec2<f32>;
    var colRays: vec3<f32>;
    var param_1: vec2<f32>;
    var i_2: i32;
    var colorWithRays: vec3<f32>;
    var phi_184_: bool;

    let _e48 = gl_GlobalInvocationID_1;
    fragCoord = bitcast<vec2<i32>>(_e48.xy);
    let _e51 = fragCoord;
    let _e54 = uboCS.resolution;
    uv_1 = (vec2<f32>(_e51) / _e54.xy);
    let _e57 = uv_1;
    param = _e57;
    let _e58 = GaussianBlur_u0028_t21_u003b_p1_u003b_vf2_u003b(skyTex, skySampler, (&param));
    fragColor = _e58;
    let _e60 = uboCS.lightDotCameraForward;
    let _e61 = (_e60 > 0f);
    phi_184_ = _e61;
    if _e61 {
        let _e63 = uboCS.enableGodRays;
        phi_184_ = (_e63 != 0u);
    }
    let _e66 = phi_184_;
    if _e66 {
        decay = 0.98f;
        density = 0.9f;
        weight = 0.07f;
        exposure = 0.45f;
        illuminationDecay = 1f;
        let _e67 = uv_1;
        textureCoord = _e67;
        let _e68 = textureCoord;
        let _e70 = uboCS.lightPosition;
        textureCoordDelta = (_e68 - _e70.xy);
        let _e73 = density;
        let _e75 = textureCoordDelta;
        textureCoordDelta = (_e75 * (_e73 / 64f));
        let _e77 = uv_1;
        param_1 = _e77;
        let _e78 = GaussianBlur_u0028_t21_u003b_p1_u003b_vf2_u003b(bloomTex, bloomSampler, (&param_1));
        colRays = (_e78.xyz * 0.4f);
        i_2 = 0i;
        loop {
            let _e81 = i_2;
            if (_e81 < 64i) {
                let _e83 = textureCoordDelta;
                let _e84 = uv_1;
                uv_1 = (_e84 - _e83);
                let _e86 = uv_1;
                let _e87 = textureSampleLevel(bloomTex, bloomSampler, _e86, 0f);
                let _e89 = illuminationDecay;
                let _e91 = weight;
                let _e93 = colRays;
                colRays = (_e93 + ((_e87.xyz * _e89) * _e91));
                let _e95 = decay;
                let _e96 = illuminationDecay;
                illuminationDecay = (_e96 * _e95);
                continue;
            } else {
                break;
            }
            continuing {
                let _e98 = i_2;
                i_2 = (_e98 + 1i);
            }
        }
        let _e100 = fragColor;
        let _e102 = colRays;
        let _e106 = exposure;
        colorWithRays = (_e100.xyz + (smoothstep(vec3(0f), vec3(1f), _e102) * _e106));
        let _e109 = fragColor;
        let _e111 = colorWithRays;
        let _e114 = uboCS.lightDotCameraForward;
        let _e116 = uboCS.lightDotCameraForward;
        let _e119 = mix(_e109.xyz, (_e111 * 0.9f), vec3((_e114 * _e116)));
        fragColor[0u] = _e119.x;
        fragColor[1u] = _e119.y;
        fragColor[2u] = _e119.z;
    }
    let _e126 = fragCoord;
    let _e127 = fragColor;
    textureStore(outFragColor, _e126, _e127);
    return;
}

@compute @workgroup_size(16, 16, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
