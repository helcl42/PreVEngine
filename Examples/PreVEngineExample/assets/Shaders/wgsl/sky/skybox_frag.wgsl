struct UniformBufferObject {
    fogColor: vec4<f32>,
    lowerLimit: vec4<f32>,
    upperLimit: vec4<f32>,
}

var<private> inTextureCoord_1: vec3<f32>;
@group(0) @binding(2) 
var cubeMap1Texture: texture_cube<f32>;
@group(0) @binding(3) 
var cubeMap1Sampler: sampler;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> outColor: vec4<f32>;

fn main_1() {
    var tc: vec3<f32>;
    var finalColor: vec4<f32>;
    var factor: f32;

    let _e17 = inTextureCoord_1[0u];
    let _e20 = inTextureCoord_1[1u];
    let _e22 = inTextureCoord_1[2u];
    tc = vec3<f32>(-(_e17), _e20, _e22);
    let _e24 = tc;
    let _e25 = textureSample(cubeMap1Texture, cubeMap1Sampler, _e24);
    finalColor = _e25;
    let _e27 = inTextureCoord_1[1u];
    let _e30 = uboFS.lowerLimit[0u];
    let _e34 = uboFS.upperLimit[0u];
    let _e37 = uboFS.lowerLimit[0u];
    factor = ((_e27 - _e30) / (_e34 - _e37));
    let _e40 = factor;
    factor = clamp(_e40, 0f, 1f);
    let _e43 = uboFS.fogColor;
    let _e44 = _e43.xyz;
    let _e49 = finalColor;
    let _e50 = factor;
    outColor = mix(vec4<f32>(_e44.x, _e44.y, _e44.z, 1f), _e49, vec4(_e50));
    return;
}

@fragment 
fn main(@location(0) inTextureCoord: vec3<f32>) -> @location(0) vec4<f32> {
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
