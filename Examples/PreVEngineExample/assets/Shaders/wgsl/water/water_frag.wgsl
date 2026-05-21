struct ShadowsCascade {
    viewProjectionMatrix: mat4x4<f32>,
    split: vec4<f32>,
}

struct Shadows {
    cascades: array<ShadowsCascade, 4>,
    enabled: u32,
    useReverseDepth: u32,
}

struct Light {
    position: vec4<f32>,
    color: vec4<f32>,
}

struct UniformBufferObject {
    shadows: Shadows,
    fogColor: vec4<f32>,
    waterColor: vec4<f32>,
    light: Light,
    nearFarClippinPlane: vec4<f32>,
    moveFactor: f32,
}

var<private> gl_FragCoord_1: vec4<f32>;
@group(0) @binding(8) 
var depthMapTexture: texture_2d<f32>;
@group(0) @binding(9) 
var depthMapSampler: sampler;
@group(0) @binding(6) 
var refractionTexture: texture_2d<f32>;
@group(0) @binding(7) 
var refractionSampler: sampler;
@group(0) @binding(4) 
var reflectionTexture: texture_2d<f32>;
@group(0) @binding(5) 
var reflectionSampler: sampler;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
@group(0) @binding(2) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(3) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inClipSpaceCoord_1: vec4<f32>;
@group(0) @binding(10) 
var dudvMapTexture: texture_2d<f32>;
@group(0) @binding(11) 
var dudvMapSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(12) 
var normalMapTexture: texture_2d<f32>;
@group(0) @binding(13) 
var normalMapSampler: sampler;
var<private> inToCameraVector_1: vec3<f32>;
var<private> outColor: vec4<f32>;
var<private> inVisibility_1: f32;

fn SampleRefraction_u0028_vf2_u003b(texCoord: ptr<function, vec2<f32>>) -> vec4<f32> {
    var refractColor: vec4<f32>;

    let _e64 = (*texCoord);
    let _e65 = textureSample(refractionTexture, refractionSampler, _e64);
    refractColor = _e65;
    let _e66 = refractColor;
    return _e66;
}

fn SampleReflection_u0028_vf2_u003b(texCoord_1: ptr<function, vec2<f32>>) -> vec4<f32> {
    var reflectColor: vec4<f32>;

    let _e64 = (*texCoord_1);
    let _e65 = textureSample(reflectionTexture, reflectionSampler, _e64);
    reflectColor = _e65;
    let _e66 = reflectColor;
    return _e66;
}

fn LinearizeDepth_u0028_f1_u003b_f1_u003b_f1_u003b(depth: ptr<function, f32>, zNear: ptr<function, f32>, zFar: ptr<function, f32>) -> f32 {
    let _e65 = (*zNear);
    let _e66 = (*zFar);
    let _e68 = (*zFar);
    let _e69 = (*depth);
    let _e70 = (*zNear);
    let _e71 = (*zFar);
    return ((_e65 * _e66) / (_e68 + (_e69 * (_e70 - _e71))));
}

fn SampleDepth_u0028_vf2_u003b(texCoords: ptr<function, vec2<f32>>) -> f32 {
    var depth_1: f32;

    let _e64 = (*texCoords);
    let _e65 = textureSample(depthMapTexture, depthMapSampler, _e64);
    depth_1 = _e65.x;
    let _e67 = depth_1;
    return _e67;
}

fn CalculateWaterDepth_u0028_vf2_u003b(texCoords_1: ptr<function, vec2<f32>>) -> f32 {
    var depth_2: f32;
    var param: vec2<f32>;
    var floorDistance: f32;
    var param_1: f32;
    var param_2: f32;
    var param_3: f32;
    var waterDistance: f32;
    var param_4: f32;
    var param_5: f32;
    var param_6: f32;

    let _e73 = (*texCoords_1);
    param = _e73;
    let _e74 = SampleDepth_u0028_vf2_u003b((&param));
    depth_2 = _e74;
    let _e75 = depth_2;
    param_1 = _e75;
    let _e78 = uboFS.nearFarClippinPlane[0u];
    param_2 = _e78;
    let _e81 = uboFS.nearFarClippinPlane[1u];
    param_3 = _e81;
    let _e82 = LinearizeDepth_u0028_f1_u003b_f1_u003b_f1_u003b((&param_1), (&param_2), (&param_3));
    floorDistance = _e82;
    let _e84 = gl_FragCoord_1[2u];
    depth_2 = _e84;
    let _e85 = depth_2;
    param_4 = _e85;
    let _e88 = uboFS.nearFarClippinPlane[0u];
    param_5 = _e88;
    let _e91 = uboFS.nearFarClippinPlane[1u];
    param_6 = _e91;
    let _e92 = LinearizeDepth_u0028_f1_u003b_f1_u003b_f1_u003b((&param_4), (&param_5), (&param_6));
    waterDistance = _e92;
    let _e93 = floorDistance;
    let _e94 = waterDistance;
    return (_e93 - _e94);
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth_3: f32;
    var phi_108_: bool;

    shadow = 1f;
    let _e72 = (*shadowCoord)[2u];
    let _e73 = (_e72 >= 0f);
    phi_108_ = _e73;
    if _e73 {
        let _e75 = (*shadowCoord)[2u];
        phi_108_ = (_e75 <= 1f);
    }
    let _e78 = phi_108_;
    if _e78 {
        let _e79 = (*shadowCoord);
        let _e81 = (*shadowCoordOffset);
        let _e82 = (_e79.xy + _e81);
        let _e83 = (*cascadeIndex);
        let _e87 = vec3<f32>(_e82.x, _e82.y, f32(_e83));
        let _e93 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e87.x, _e87.y), i32(_e87.z), 0f);
        depth_3 = _e93.x;
        let _e95 = (*useReverseDepth);
        if (_e95 != 0u) {
            let _e97 = depth_3;
            let _e99 = (*shadowCoord)[2u];
            let _e100 = (*depthBias);
            if (_e97 > (_e99 + _e100)) {
                shadow = 0.2f;
            }
        } else {
            let _e103 = depth_3;
            let _e105 = (*shadowCoord)[2u];
            let _e106 = (*depthBias);
            if (_e103 < (_e105 - _e106)) {
                shadow = 0.2f;
            }
        }
    }
    let _e109 = shadow;
    return _e109;
}

fn GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1: texture_2d_array<f32>, depthSampler_1: sampler, shadowCoord_1: ptr<function, vec4<f32>>, cascadeIndex_1: ptr<function, u32>, depthBias_1: ptr<function, f32>, useReverseDepth_1: ptr<function, u32>) -> f32 {
    var textureDim: vec2<f32>;
    var texelSize: vec2<f32>;
    var shadow_1: f32;
    var offset: vec2<f32>;
    var param_7: vec4<f32>;
    var param_8: vec2<f32>;
    var param_9: u32;
    var param_10: f32;
    var param_11: u32;
    var param_12: vec4<f32>;
    var param_13: vec2<f32>;
    var param_14: u32;
    var param_15: f32;
    var param_16: u32;
    var param_17: vec4<f32>;
    var param_18: vec2<f32>;
    var param_19: u32;
    var param_20: f32;
    var param_21: u32;
    var param_22: vec4<f32>;
    var param_23: vec2<f32>;
    var param_24: u32;
    var param_25: f32;
    var param_26: u32;

    let _e92 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e92).xy);
    let _e96 = textureDim;
    texelSize = (vec2(1f) / _e96);
    shadow_1 = 0f;
    let _e99 = gl_FragCoord_1;
    offset = (fract((_e99.xy * 0.5f)) + vec2(0.25f));
    let _e106 = offset[1u];
    if (_e106 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e109 = texelSize;
    let _e110 = offset;
    let _e113 = (*shadowCoord_1);
    param_7 = _e113;
    param_8 = (_e109 * (_e110 + vec2<f32>(-1.5f, 0.5f)));
    let _e114 = (*cascadeIndex_1);
    param_9 = _e114;
    let _e115 = (*depthBias_1);
    param_10 = _e115;
    let _e116 = (*useReverseDepth_1);
    param_11 = _e116;
    let _e117 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_7), (&param_8), (&param_9), (&param_10), (&param_11));
    let _e118 = texelSize;
    let _e119 = offset;
    let _e122 = (*shadowCoord_1);
    param_12 = _e122;
    param_13 = (_e118 * (_e119 + vec2<f32>(0.5f, 0.5f)));
    let _e123 = (*cascadeIndex_1);
    param_14 = _e123;
    let _e124 = (*depthBias_1);
    param_15 = _e124;
    let _e125 = (*useReverseDepth_1);
    param_16 = _e125;
    let _e126 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_12), (&param_13), (&param_14), (&param_15), (&param_16));
    let _e128 = texelSize;
    let _e129 = offset;
    let _e132 = (*shadowCoord_1);
    param_17 = _e132;
    param_18 = (_e128 * (_e129 + vec2<f32>(-1.5f, -1.5f)));
    let _e133 = (*cascadeIndex_1);
    param_19 = _e133;
    let _e134 = (*depthBias_1);
    param_20 = _e134;
    let _e135 = (*useReverseDepth_1);
    param_21 = _e135;
    let _e136 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_17), (&param_18), (&param_19), (&param_20), (&param_21));
    let _e138 = texelSize;
    let _e139 = offset;
    let _e142 = (*shadowCoord_1);
    param_22 = _e142;
    param_23 = (_e138 * (_e139 + vec2<f32>(0.5f, -1.5f)));
    let _e143 = (*cascadeIndex_1);
    param_24 = _e143;
    let _e144 = (*depthBias_1);
    param_25 = _e144;
    let _e145 = (*useReverseDepth_1);
    param_26 = _e145;
    let _e146 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_22), (&param_23), (&param_24), (&param_25), (&param_26));
    shadow_1 = ((((_e117 + _e126) + _e136) + _e146) * 0.25f);
    let _e149 = shadow_1;
    return _e149;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2: texture_2d_array<f32>, depthSampler_2: sampler, shadowCoord_2: ptr<function, vec4<f32>>, cascadeIndex_2: ptr<function, u32>, depthBias_2: ptr<function, f32>, useReverseDepth_2: ptr<function, u32>) -> f32 {
    var shadow_2: f32;
    var param_27: vec4<f32>;
    var param_28: u32;
    var param_29: f32;
    var param_30: u32;
    var param_31: vec4<f32>;
    var param_32: vec2<f32>;
    var param_33: u32;
    var param_34: f32;
    var param_35: u32;

    shadow_2 = 1f;
    if true {
        let _e78 = (*shadowCoord_2);
        param_27 = _e78;
        let _e79 = (*cascadeIndex_2);
        param_28 = _e79;
        let _e80 = (*depthBias_2);
        param_29 = _e80;
        let _e81 = (*useReverseDepth_2);
        param_30 = _e81;
        let _e82 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_27), (&param_28), (&param_29), (&param_30));
        shadow_2 = _e82;
    } else {
        let _e83 = (*shadowCoord_2);
        param_31 = _e83;
        param_32 = vec2<f32>(0f, 0f);
        let _e84 = (*cascadeIndex_2);
        param_33 = _e84;
        let _e85 = (*depthBias_2);
        param_34 = _e85;
        let _e86 = (*useReverseDepth_2);
        param_35 = _e86;
        let _e87 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_31), (&param_32), (&param_33), (&param_34), (&param_35));
        shadow_2 = _e87;
    }
    let _e88 = shadow_2;
    return _e88;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_3: texture_2d_array<f32>, depthSampler_3: sampler, shadows: ptr<function, Shadows>, viewPosition: ptr<function, vec3<f32>>, worldPosition: ptr<function, vec3<f32>>, depthBias_3: ptr<function, f32>) -> f32 {
    var shadow_3: f32;
    var bias: f32;
    var cascadeIndex_3: u32;
    var i: u32;
    var shadowCoord_3: vec4<f32>;
    var normalizedShadowCoord: vec4<f32>;
    var param_36: vec4<f32>;
    var param_37: u32;
    var param_38: f32;
    var param_39: u32;

    shadow_3 = 1f;
    let _e78 = (*depthBias_3);
    bias = _e78;
    let _e80 = (*shadows).enabled;
    if (_e80 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e82 = i;
            if (_e82 < 3u) {
                let _e85 = (*shadows).useReverseDepth;
                if (_e85 != 0u) {
                    let _e88 = (*viewPosition)[2u];
                    let _e89 = i;
                    let _e94 = (*shadows).cascades[_e89].split[0u];
                    if (_e88 > _e94) {
                        let _e96 = i;
                        cascadeIndex_3 = (_e96 + 1u);
                        let _e98 = bias;
                        bias = (_e98 / 1.5f);
                    }
                } else {
                    let _e101 = (*viewPosition)[2u];
                    let _e102 = i;
                    let _e107 = (*shadows).cascades[_e102].split[0u];
                    if (_e101 < _e107) {
                        let _e109 = i;
                        cascadeIndex_3 = (_e109 + 1u);
                        let _e111 = bias;
                        bias = (_e111 / 1.5f);
                    }
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e113 = i;
                i = (_e113 + bitcast<u32>(1i));
            }
        }
        let _e116 = cascadeIndex_3;
        let _e120 = (*shadows).cascades[_e116].viewProjectionMatrix;
        let _e121 = (*worldPosition);
        shadowCoord_3 = (_e120 * vec4<f32>(_e121.x, _e121.y, _e121.z, 1f));
        let _e127 = shadowCoord_3;
        let _e129 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e127 / vec4(_e129));
        let _e132 = normalizedShadowCoord;
        param_36 = _e132;
        let _e133 = cascadeIndex_3;
        param_37 = _e133;
        let _e134 = bias;
        param_38 = _e134;
        let _e136 = (*shadows).useReverseDepth;
        param_39 = _e136;
        let _e137 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_36), (&param_37), (&param_38), (&param_39));
        shadow_3 = _e137;
    }
    let _e138 = shadow_3;
    return _e138;
}

fn main_1() {
    var shadow_4: f32;
    var param_40: Shadows;
    var param_41: vec3<f32>;
    var param_42: vec3<f32>;
    var param_43: f32;
    var normalizedDeviceSapceCoord: vec2<f32>;
    var reflectTexCoord: vec2<f32>;
    var refractTexCoord: vec2<f32>;
    var waterDepth: f32;
    var param_44: vec2<f32>;
    var distortedTexCoords: vec2<f32>;
    var totalDistortion: vec2<f32>;
    var reflectColor_1: vec4<f32>;
    var param_45: vec2<f32>;
    var refractColor_1: vec4<f32>;
    var param_46: vec2<f32>;
    var finalWaterColor: vec4<f32>;
    var normalMapColor: vec4<f32>;
    var normal: vec3<f32>;
    var viewVector: vec3<f32>;
    var refractiveFactor: f32;
    var toLightVector: vec3<f32>;
    var reflectedLight: vec3<f32>;
    var specular: f32;
    var specularHighlights: vec3<f32>;
    var baseResultColor: vec4<f32>;

    let _e89 = uboFS.shadows;
    param_40.cascades[0i].viewProjectionMatrix = _e89.cascades[0].viewProjectionMatrix;
    param_40.cascades[0i].split = _e89.cascades[0].split;
    param_40.cascades[1i].viewProjectionMatrix = _e89.cascades[1].viewProjectionMatrix;
    param_40.cascades[1i].split = _e89.cascades[1].split;
    param_40.cascades[2i].viewProjectionMatrix = _e89.cascades[2].viewProjectionMatrix;
    param_40.cascades[2i].split = _e89.cascades[2].split;
    param_40.cascades[3i].viewProjectionMatrix = _e89.cascades[3].viewProjectionMatrix;
    param_40.cascades[3i].split = _e89.cascades[3].split;
    param_40.enabled = _e89.enabled;
    param_40.useReverseDepth = _e89.useReverseDepth;
    let _e120 = inViewPosition_1;
    param_41 = _e120;
    let _e121 = inWorldPosition_1;
    param_42 = _e121;
    param_43 = 0.005f;
    let _e122 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_40), (&param_41), (&param_42), (&param_43));
    shadow_4 = _e122;
    let _e123 = shadow_4;
    if (_e123 < 0.999f) {
        shadow_4 = 0f;
    }
    let _e125 = inClipSpaceCoord_1;
    let _e128 = inClipSpaceCoord_1[3u];
    normalizedDeviceSapceCoord = (((_e125.xy / vec2(_e128)) / vec2(2f)) + vec2(0.5f));
    let _e136 = normalizedDeviceSapceCoord[0u];
    let _e138 = normalizedDeviceSapceCoord[1u];
    reflectTexCoord = vec2<f32>(_e136, (1f - _e138));
    let _e142 = normalizedDeviceSapceCoord[0u];
    let _e144 = normalizedDeviceSapceCoord[1u];
    refractTexCoord = vec2<f32>(_e142, _e144);
    let _e146 = refractTexCoord;
    param_44 = _e146;
    let _e147 = CalculateWaterDepth_u0028_vf2_u003b((&param_44));
    waterDepth = _e147;
    let _e149 = inTextureCoord_1[0u];
    let _e151 = uboFS.moveFactor;
    let _e154 = inTextureCoord_1[1u];
    let _e156 = textureSample(dudvMapTexture, dudvMapSampler, vec2<f32>((_e149 + _e151), _e154));
    distortedTexCoords = (_e156.xy * 0.1f);
    let _e159 = inTextureCoord_1;
    let _e161 = distortedTexCoords[0u];
    let _e163 = distortedTexCoords[1u];
    let _e165 = uboFS.moveFactor;
    distortedTexCoords = (_e159 + vec2<f32>(_e161, (_e163 + _e165)));
    let _e169 = distortedTexCoords;
    let _e170 = textureSample(dudvMapTexture, dudvMapSampler, _e169);
    let _e176 = waterDepth;
    totalDistortion = ((((_e170.xy * 2f) - vec2(1f)) * 0.04f) * clamp((_e176 / 12f), 0f, 1f));
    let _e180 = totalDistortion;
    let _e181 = reflectTexCoord;
    reflectTexCoord = (_e181 + _e180);
    let _e183 = reflectTexCoord;
    reflectTexCoord = clamp(_e183, vec2(0.001f), vec2(0.999f));
    let _e187 = totalDistortion;
    let _e188 = refractTexCoord;
    refractTexCoord = (_e188 + _e187);
    let _e190 = refractTexCoord;
    refractTexCoord = clamp(_e190, vec2(0.001f), vec2(0.999f));
    let _e194 = reflectTexCoord;
    param_45 = _e194;
    let _e195 = SampleReflection_u0028_vf2_u003b((&param_45));
    reflectColor_1 = _e195;
    let _e196 = refractTexCoord;
    param_46 = _e196;
    let _e197 = SampleRefraction_u0028_vf2_u003b((&param_46));
    refractColor_1 = _e197;
    let _e199 = uboFS.waterColor;
    let _e200 = shadow_4;
    finalWaterColor = (_e199 * _e200);
    let _e202 = distortedTexCoords;
    let _e203 = textureSample(normalMapTexture, normalMapSampler, _e202);
    normalMapColor = _e203;
    let _e205 = normalMapColor[0u];
    let _e209 = normalMapColor[1u];
    let _e212 = normalMapColor[2u];
    normal = vec3<f32>(((_e205 * 2f) - 1f), (_e209 * 3f), ((_e212 * 2f) - 1f));
    let _e216 = normal;
    normal = normalize(_e216);
    let _e218 = inToCameraVector_1;
    viewVector = normalize(_e218);
    let _e220 = viewVector;
    let _e221 = normal;
    refractiveFactor = dot(_e220, _e221);
    let _e223 = refractiveFactor;
    refractiveFactor = pow(_e223, 0.7f);
    let _e225 = refractiveFactor;
    refractiveFactor = clamp(_e225, 0.001f, 0.999f);
    let _e229 = uboFS.light.position;
    let _e231 = inWorldPosition_1;
    toLightVector = (_e229.xyz - _e231);
    let _e233 = toLightVector;
    let _e236 = normal;
    reflectedLight = reflect(normalize(-(_e233)), _e236);
    let _e238 = reflectedLight;
    let _e239 = viewVector;
    specular = max(dot(_e238, _e239), 0f);
    let _e242 = specular;
    specular = pow(_e242, 20f);
    let _e246 = uboFS.light.color;
    let _e248 = specular;
    let _e251 = waterDepth;
    specularHighlights = (((_e246.xyz * _e248) * 0.45f) * clamp((_e251 / 5f), 0f, 1f));
    let _e255 = reflectColor_1;
    let _e256 = refractColor_1;
    let _e257 = refractiveFactor;
    baseResultColor = mix(_e255, _e256, vec4(_e257));
    let _e260 = baseResultColor;
    let _e261 = finalWaterColor;
    let _e264 = specularHighlights;
    baseResultColor = clamp((mix(_e260, _e261, vec4(0.2f)) + vec4<f32>(_e264.x, _e264.y, _e264.z, 1f)), vec4(0f), vec4(1f));
    let _e273 = baseResultColor;
    outColor = _e273;
    let _e274 = waterDepth;
    outColor[3u] = clamp((_e274 / 5f), 0f, 1f);
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(3) inViewPosition: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(0) inClipSpaceCoord: vec4<f32>, @location(1) inTextureCoord: vec2<f32>, @location(4) inToCameraVector: vec3<f32>, @location(5) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inViewPosition_1 = inViewPosition;
    inWorldPosition_1 = inWorldPosition;
    inClipSpaceCoord_1 = inClipSpaceCoord;
    inTextureCoord_1 = inTextureCoord;
    inToCameraVector_1 = inToCameraVector;
    inVisibility_1 = inVisibility;
    main_1();
    let _e15 = outColor;
    return _e15;
}
