struct PushConsts {
    cascadeIndex: u32,
    nearClippingPlane: f32,
    farClippingPlane: f32,
}

@group(0) @binding(0) 
var depthTexture: texture_2d_array<f32>;
@group(0) @binding(1) 
var depthSampler: sampler;
var<private> textureCoord_1: vec2<f32>;
var<immediate> pushConsts: PushConsts;
var<private> outFragColor: vec4<f32>;

fn LinearizeDepth_u0028_f1_u003b_f1_u003b_f1_u003b(near: f32, far: f32, depth: f32) -> f32 {
    var n: f32;
    var f: f32;
    var z: f32;

    n = near;
    f = far;
    z = depth;
    let _e17 = n;
    let _e19 = f;
    let _e20 = n;
    let _e22 = z;
    let _e23 = f;
    let _e24 = n;
    return ((2f * _e17) / ((_e19 + _e20) - (_e22 * (_e23 - _e24))));
}

fn main_1() {
    var depth_1: f32;

    let _e12 = textureCoord_1;
    let _e14 = pushConsts.cascadeIndex;
    let _e18 = vec3<f32>(_e12.x, _e12.y, f32(_e14));
    let _e24 = textureSample(depthTexture, depthSampler, vec2<f32>(_e18.x, _e18.y), i32(_e18.z));
    depth_1 = _e24.x;
    let _e27 = pushConsts.nearClippingPlane;
    let _e29 = pushConsts.farClippingPlane;
    let _e30 = depth_1;
    let _e31 = LinearizeDepth_u0028_f1_u003b_f1_u003b_f1_u003b(_e27, _e29, _e30);
    let _e33 = vec3((1f - _e31));
    outFragColor = vec4<f32>(_e33.x, _e33.y, _e33.z, 1f);
    return;
}

@fragment 
fn main(@location(0) textureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    textureCoord_1 = textureCoord;
    main_1();
    let _e3 = outFragColor;
    return _e3;
}
