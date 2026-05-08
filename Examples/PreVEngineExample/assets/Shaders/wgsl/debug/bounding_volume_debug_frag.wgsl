struct UniformBufferObject {
    color: vec4<f32>,
    selectedColor: vec4<f32>,
    selected: u32,
}

@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> outColor: vec4<f32>;

fn main_1() {
    var resultColor: vec4<f32>;

    let _e8 = uboFS.color;
    resultColor = _e8;
    let _e10 = uboFS.selected;
    if (_e10 != 0u) {
        let _e13 = uboFS.selectedColor;
        resultColor = _e13;
    }
    let _e14 = resultColor;
    outColor = _e14;
    return;
}

@fragment 
fn main() -> @location(0) vec4<f32> {
    main_1();
    let _e1 = outColor;
    return _e1;
}
