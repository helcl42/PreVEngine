struct UniformBufferObject {
    color: vec4<f32>,
    width: vec4<f32>,
    edge: vec4<f32>,
    bias: vec4<f32>,
    hasEffect: u32,
    borderWidth: vec4<f32>,
    borderEdge: vec4<f32>,
    outlineColor: vec4<f32>,
    outlineOffset: vec4<f32>,
}

@group(0) @binding(2) 
var alphaTexture: texture_2d<f32>;
@group(0) @binding(3) 
var alphaSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> outColor: vec4<f32>;

fn main_1() {
    var distance_: f32;
    var alpha: f32;
    var distance2_: f32;
    var outlineAlpha: f32;
    var overallAlpha: f32;
    var overallColor: vec3<f32>;

    let _e25 = inTextureCoord_1[0u];
    let _e28 = uboFS.bias[0u];
    let _e31 = inTextureCoord_1[1u];
    let _e34 = uboFS.bias[1u];
    let _e37 = textureSample(alphaTexture, alphaSampler, vec2<f32>((_e25 + _e28), (_e31 + _e34)));
    distance_ = (1f - _e37.w);
    let _e42 = uboFS.width[0u];
    let _e45 = uboFS.width[0u];
    let _e48 = uboFS.edge[0u];
    let _e50 = distance_;
    alpha = (1f - smoothstep(_e42, (_e45 + _e48), _e50));
    let _e54 = uboFS.hasEffect;
    if (_e54 != 0u) {
        let _e57 = inTextureCoord_1[0u];
        let _e59 = inTextureCoord_1[1u];
        let _e62 = uboFS.outlineOffset;
        let _e65 = textureSample(alphaTexture, alphaSampler, (vec2<f32>(_e57, _e59) + _e62.xy));
        distance2_ = (1f - _e65.w);
        let _e70 = uboFS.borderWidth[0u];
        let _e73 = uboFS.borderWidth[0u];
        let _e76 = uboFS.borderEdge[0u];
        let _e78 = distance2_;
        outlineAlpha = (1f - smoothstep(_e70, (_e73 + _e76), _e78));
        let _e81 = alpha;
        let _e82 = alpha;
        let _e84 = outlineAlpha;
        overallAlpha = (_e81 + ((1f - _e82) * _e84));
        let _e88 = uboFS.outlineColor;
        let _e91 = uboFS.color;
        let _e93 = alpha;
        let _e94 = overallAlpha;
        overallColor = mix(_e88.xyz, _e91.xyz, vec3((_e93 / _e94)));
        let _e98 = overallColor;
        let _e99 = overallAlpha;
        let _e102 = uboFS.color[3u];
        outColor = vec4<f32>(_e98.x, _e98.y, _e98.z, (_e99 * _e102));
    } else {
        let _e109 = uboFS.color;
        let _e110 = _e109.xyz;
        let _e111 = alpha;
        let _e114 = uboFS.color[3u];
        outColor = vec4<f32>(_e110.x, _e110.y, _e110.z, (_e111 * _e114));
    }
    return;
}

@fragment 
fn main(@location(0) inTextureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e3 = outColor;
    return _e3;
}
