struct UniformBufferObject {
    color: vec4<f32>,
}

var<private> outColor: vec4<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;

fn main_1() {
    let _e4 = uboFS.color;
    outColor = _e4;
    return;
}

@fragment 
fn main() -> @location(0) vec4<f32> {
    main_1();
    let _e1 = outColor;
    return _e1;
}
