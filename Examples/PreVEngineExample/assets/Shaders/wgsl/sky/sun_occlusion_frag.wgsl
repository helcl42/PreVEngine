var<private> outColor: vec4<f32>;

fn main_1() {
    outColor = vec4<f32>(1f, 1f, 1f, 0f);
    return;
}

@fragment 
fn main() -> @location(0) vec4<f32> {
    main_1();
    let _e1 = outColor;
    return _e1;
}
