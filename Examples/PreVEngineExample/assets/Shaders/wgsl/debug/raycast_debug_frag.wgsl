struct UniformBufferObject {
    alpha: f32,
}

var<private> outColor: vec4<f32>;
var<private> inColor_1: vec3<f32>;
@group(0) @binding(2) 
var<uniform> uboFS: UniformBufferObject;

fn main_1() {
    let _e4 = inColor_1;
    let _e6 = uboFS.alpha;
    outColor = vec4<f32>(_e4.x, _e4.y, _e4.z, _e6);
    return;
}

@fragment 
fn main(@location(0) inColor: vec3<f32>) -> @location(0) vec4<f32> {
    inColor_1 = inColor;
    main_1();
    let _e3 = outColor;
    return _e3;
}
