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
    attenuation: vec4<f32>,
}

struct Lightning {
    lights: array<Light, 4>,
    realCountOfLights: u32,
    ambientFactor: f32,
}

struct Material {
    color: vec4<f32>,
    shineDamper: f32,
    reflectivity: f32,
}

struct UniformBufferObject {
    shadows: Shadows,
    lightning: Lightning,
    material: array<Material, 4>,
    fogColor: vec4<f32>,
    selectedColor: vec4<f32>,
    selected: u32,
    castedByShadows: u32,
    minHeight: f32,
    maxHeight: f32,
    heightSteps: array<vec4<f32>, 4>,
    heightTransitionRange: f32,
}

var<private> gl_FragCoord_1: vec4<f32>;
@group(0) @binding(2) 
var colorTexture0_: texture_2d<f32>;
@group(0) @binding(6) 
var colorSampler: sampler;
@group(0) @binding(3) 
var colorTexture1_: texture_2d<f32>;
@group(0) @binding(4) 
var colorTexture2_: texture_2d<f32>;
@group(0) @binding(5) 
var colorTexture3_: texture_2d<f32>;
var<private> inToLightVector0_1: vec3<f32>;
var<private> inToLightVector1_1: vec3<f32>;
var<private> inToLightVector2_1: vec3<f32>;
var<private> inToLightVector3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(7) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(8) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inToCameraVector_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e69 = (*toLightVector);
    lightDirection = -(_e69);
    let _e71 = lightDirection;
    let _e72 = (*normal);
    reflectedLightDirection = reflect(_e71, _e72);
    if true {
        let _e74 = (*toLightVector);
        let _e75 = (*toCameraVector);
        halfwayDir = normalize((_e74 + _e75));
        let _e78 = (*normal);
        let _e79 = halfwayDir;
        specularFactor = max(dot(_e78, _e79), 0f);
        let _e82 = (*shineDamper);
        shinePower = (_e82 * 2.4f);
    } else {
        let _e84 = reflectedLightDirection;
        let _e85 = (*toCameraVector);
        specularFactor = max(dot(_e84, _e85), 0f);
        let _e88 = (*shineDamper);
        shinePower = _e88;
    }
    let _e89 = specularFactor;
    let _e90 = shinePower;
    dampedFactor = pow(_e89, _e90);
    let _e92 = dampedFactor;
    let _e93 = (*reflectivity);
    let _e95 = (*lightColor);
    let _e97 = (*attenuationFactor);
    return ((_e95 * (_e92 * _e93)) / vec3(_e97));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e62 = (*normal_1);
    let _e63 = (*toLightVector_1);
    nDotL = dot(_e62, _e63);
    let _e65 = nDotL;
    brightness = max(_e65, 0f);
    let _e67 = brightness;
    let _e68 = (*lightColor_1);
    let _e70 = (*attenuationFactor_1);
    return ((_e68 * _e67) / vec3(_e70));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e60 = (*toLightVector_2);
    toLightDistance = length(_e60);
    let _e63 = (*attenuation)[0u];
    let _e65 = (*attenuation)[1u];
    let _e66 = toLightDistance;
    let _e70 = (*attenuation)[2u];
    let _e71 = toLightDistance;
    let _e73 = toLightDistance;
    attenuationFactor_2 = ((_e63 + (_e65 * _e66)) + ((_e70 * _e71) * _e73));
    let _e76 = attenuationFactor_2;
    return _e76;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_104_: bool;

    shadow = 1f;
    let _e66 = (*shadowCoord)[2u];
    let _e67 = (_e66 >= 0f);
    phi_104_ = _e67;
    if _e67 {
        let _e69 = (*shadowCoord)[2u];
        phi_104_ = (_e69 <= 1f);
    }
    let _e72 = phi_104_;
    if _e72 {
        let _e73 = (*shadowCoord);
        let _e75 = (*shadowCoordOffset);
        let _e76 = (_e73.xy + _e75);
        let _e77 = (*cascadeIndex);
        let _e81 = vec3<f32>(_e76.x, _e76.y, f32(_e77));
        let _e87 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e81.x, _e81.y), i32(_e81.z), 0f);
        depth = _e87.x;
        let _e89 = (*useReverseDepth);
        if (_e89 != 0u) {
            let _e91 = depth;
            let _e93 = (*shadowCoord)[2u];
            let _e94 = (*depthBias);
            if (_e91 > (_e93 + _e94)) {
                shadow = 0.2f;
            }
        } else {
            let _e97 = depth;
            let _e99 = (*shadowCoord)[2u];
            let _e100 = (*depthBias);
            if (_e97 < (_e99 - _e100)) {
                shadow = 0.2f;
            }
        }
    }
    let _e103 = shadow;
    return _e103;
}

fn GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1: texture_2d_array<f32>, depthSampler_1: sampler, shadowCoord_1: ptr<function, vec4<f32>>, cascadeIndex_1: ptr<function, u32>, depthBias_1: ptr<function, f32>, useReverseDepth_1: ptr<function, u32>) -> f32 {
    var textureDim: vec2<f32>;
    var texelSize: vec2<f32>;
    var shadow_1: f32;
    var offset: vec2<f32>;
    var param: vec4<f32>;
    var param_1: vec2<f32>;
    var param_2: u32;
    var param_3: f32;
    var param_4: u32;
    var param_5: vec4<f32>;
    var param_6: vec2<f32>;
    var param_7: u32;
    var param_8: f32;
    var param_9: u32;
    var param_10: vec4<f32>;
    var param_11: vec2<f32>;
    var param_12: u32;
    var param_13: f32;
    var param_14: u32;
    var param_15: vec4<f32>;
    var param_16: vec2<f32>;
    var param_17: u32;
    var param_18: f32;
    var param_19: u32;

    let _e86 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e86).xy);
    let _e90 = textureDim;
    texelSize = (vec2(1f) / _e90);
    shadow_1 = 0f;
    let _e93 = gl_FragCoord_1;
    offset = (fract((_e93.xy * 0.5f)) + vec2(0.25f));
    let _e100 = offset[1u];
    if (_e100 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e103 = texelSize;
    let _e104 = offset;
    let _e107 = (*shadowCoord_1);
    param = _e107;
    param_1 = (_e103 * (_e104 + vec2<f32>(-1.5f, 0.5f)));
    let _e108 = (*cascadeIndex_1);
    param_2 = _e108;
    let _e109 = (*depthBias_1);
    param_3 = _e109;
    let _e110 = (*useReverseDepth_1);
    param_4 = _e110;
    let _e111 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e112 = texelSize;
    let _e113 = offset;
    let _e116 = (*shadowCoord_1);
    param_5 = _e116;
    param_6 = (_e112 * (_e113 + vec2<f32>(0.5f, 0.5f)));
    let _e117 = (*cascadeIndex_1);
    param_7 = _e117;
    let _e118 = (*depthBias_1);
    param_8 = _e118;
    let _e119 = (*useReverseDepth_1);
    param_9 = _e119;
    let _e120 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e122 = texelSize;
    let _e123 = offset;
    let _e126 = (*shadowCoord_1);
    param_10 = _e126;
    param_11 = (_e122 * (_e123 + vec2<f32>(-1.5f, -1.5f)));
    let _e127 = (*cascadeIndex_1);
    param_12 = _e127;
    let _e128 = (*depthBias_1);
    param_13 = _e128;
    let _e129 = (*useReverseDepth_1);
    param_14 = _e129;
    let _e130 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e132 = texelSize;
    let _e133 = offset;
    let _e136 = (*shadowCoord_1);
    param_15 = _e136;
    param_16 = (_e132 * (_e133 + vec2<f32>(0.5f, -1.5f)));
    let _e137 = (*cascadeIndex_1);
    param_17 = _e137;
    let _e138 = (*depthBias_1);
    param_18 = _e138;
    let _e139 = (*useReverseDepth_1);
    param_19 = _e139;
    let _e140 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e111 + _e120) + _e130) + _e140) * 0.25f);
    let _e143 = shadow_1;
    return _e143;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2: texture_2d_array<f32>, depthSampler_2: sampler, shadowCoord_2: ptr<function, vec4<f32>>, cascadeIndex_2: ptr<function, u32>, depthBias_2: ptr<function, f32>, useReverseDepth_2: ptr<function, u32>) -> f32 {
    var shadow_2: f32;
    var param_20: vec4<f32>;
    var param_21: u32;
    var param_22: f32;
    var param_23: u32;
    var param_24: vec4<f32>;
    var param_25: vec2<f32>;
    var param_26: u32;
    var param_27: f32;
    var param_28: u32;

    shadow_2 = 1f;
    if true {
        let _e72 = (*shadowCoord_2);
        param_20 = _e72;
        let _e73 = (*cascadeIndex_2);
        param_21 = _e73;
        let _e74 = (*depthBias_2);
        param_22 = _e74;
        let _e75 = (*useReverseDepth_2);
        param_23 = _e75;
        let _e76 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e76;
    } else {
        let _e77 = (*shadowCoord_2);
        param_24 = _e77;
        param_25 = vec2<f32>(0f, 0f);
        let _e78 = (*cascadeIndex_2);
        param_26 = _e78;
        let _e79 = (*depthBias_2);
        param_27 = _e79;
        let _e80 = (*useReverseDepth_2);
        param_28 = _e80;
        let _e81 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e81;
    }
    let _e82 = shadow_2;
    return _e82;
}

fn GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_3: texture_2d_array<f32>, depthSampler_3: sampler, shadows: ptr<function, Shadows>, viewPosition: ptr<function, vec3<f32>>, worldPosition: ptr<function, vec3<f32>>, depthBias_3: ptr<function, f32>) -> f32 {
    var shadow_3: f32;
    var bias: f32;
    var cascadeIndex_3: u32;
    var i: u32;
    var shadowCoord_3: vec4<f32>;
    var normalizedShadowCoord: vec4<f32>;
    var param_29: vec4<f32>;
    var param_30: u32;
    var param_31: f32;
    var param_32: u32;

    shadow_3 = 1f;
    let _e72 = (*depthBias_3);
    bias = _e72;
    let _e74 = (*shadows).enabled;
    if (_e74 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e76 = i;
            if (_e76 < 3u) {
                let _e79 = (*viewPosition)[2u];
                let _e80 = i;
                let _e85 = (*shadows).cascades[_e80].split[0u];
                if (_e79 < _e85) {
                    let _e87 = i;
                    cascadeIndex_3 = (_e87 + 1u);
                    let _e89 = bias;
                    bias = (_e89 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e91 = i;
                i = (_e91 + bitcast<u32>(1i));
            }
        }
        let _e94 = cascadeIndex_3;
        let _e98 = (*shadows).cascades[_e94].viewProjectionMatrix;
        let _e99 = (*worldPosition);
        shadowCoord_3 = (_e98 * vec4<f32>(_e99.x, _e99.y, _e99.z, 1f));
        let _e105 = shadowCoord_3;
        let _e107 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e105 / vec4(_e107));
        let _e110 = normalizedShadowCoord;
        param_29 = _e110;
        let _e111 = cascadeIndex_3;
        param_30 = _e111;
        let _e112 = bias;
        param_31 = _e112;
        let _e114 = (*shadows).useReverseDepth;
        param_32 = _e114;
        let _e115 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e115;
    }
    let _e116 = shadow_3;
    return _e116;
}

fn sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e60 = (*idx);
    if (_e60 == 0u) {
        let _e62 = (*uv);
        let _e63 = (*ddx);
        let _e64 = (*ddy);
        let _e65 = textureSampleGrad(colorTexture0_, colorSampler, _e62, _e63, _e64);
        return _e65;
    } else {
        let _e66 = (*idx);
        if (_e66 == 1u) {
            let _e68 = (*uv);
            let _e69 = (*ddx);
            let _e70 = (*ddy);
            let _e71 = textureSampleGrad(colorTexture1_, colorSampler, _e68, _e69, _e70);
            return _e71;
        } else {
            let _e72 = (*idx);
            if (_e72 == 2u) {
                let _e74 = (*uv);
                let _e75 = (*ddx);
                let _e76 = (*ddy);
                let _e77 = textureSampleGrad(colorTexture2_, colorSampler, _e74, _e75, _e76);
                return _e77;
            } else {
                let _e78 = (*uv);
                let _e79 = (*ddx);
                let _e80 = (*ddy);
                let _e81 = textureSampleGrad(colorTexture3_, colorSampler, _e78, _e79, _e80);
                return _e81;
            }
        }
    }
}

fn main_1() {
    var inToLightVector_arr: array<vec3<f32>, 4>;
    var heightRange: f32;
    var normalizedHeight: f32;
    var textureColor: vec4<f32>;
    var shineDamper_1: f32;
    var reflectivity_1: f32;
    var ddx_1: vec2<f32>;
    var ddy_1: vec2<f32>;
    var i_1: u32;
    var ratio: f32;
    var color1_: vec4<f32>;
    var param_33: u32;
    var param_34: vec2<f32>;
    var param_35: vec2<f32>;
    var param_36: vec2<f32>;
    var color2_: vec4<f32>;
    var param_37: u32;
    var param_38: vec2<f32>;
    var param_39: vec2<f32>;
    var param_40: vec2<f32>;
    var shineDamper1_: f32;
    var shineDamper2_: f32;
    var reflectivity1_: f32;
    var reflectivity2_: f32;
    var param_41: u32;
    var param_42: vec2<f32>;
    var param_43: vec2<f32>;
    var param_44: vec2<f32>;
    var param_45: u32;
    var param_46: vec2<f32>;
    var param_47: vec2<f32>;
    var param_48: vec2<f32>;
    var shadow_4: f32;
    var param_49: Shadows;
    var param_50: vec3<f32>;
    var param_51: vec3<f32>;
    var param_52: f32;
    var unitNormal: vec3<f32>;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_2: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_53: vec3<f32>;
    var param_54: vec3<f32>;
    var param_55: vec3<f32>;
    var param_56: vec3<f32>;
    var param_57: vec3<f32>;
    var param_58: f32;
    var param_59: vec3<f32>;
    var param_60: vec3<f32>;
    var param_61: vec3<f32>;
    var param_62: vec3<f32>;
    var param_63: f32;
    var param_64: f32;
    var param_65: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;
    var phi_578_: bool;

    let _e117 = inToLightVector0_1;
    let _e118 = inToLightVector1_1;
    let _e119 = inToLightVector2_1;
    let _e120 = inToLightVector3_1;
    inToLightVector_arr = array<vec3<f32>, 4>(_e117, _e118, _e119, _e120);
    let _e122 = inClipDistance_1;
    if (_e122 < 0f) {
        discard;
    }
    let _e125 = uboFS.maxHeight;
    let _e128 = uboFS.minHeight;
    heightRange = (abs(_e125) + abs(_e128));
    let _e132 = inWorldPosition_1[1u];
    let _e134 = uboFS.minHeight;
    let _e137 = heightRange;
    normalizedHeight = ((_e132 + abs(_e134)) / _e137);
    textureColor = vec4<f32>(1f, 1f, 0f, 1f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e139 = inTextureCoord_1;
    let _e140 = dpdx(_e139);
    ddx_1 = _e140;
    let _e141 = inTextureCoord_1;
    let _e142 = dpdy(_e141);
    ddy_1 = _e142;
    i_1 = 0u;
    loop {
        let _e143 = i_1;
        if (_e143 < 4u) {
            let _e145 = i_1;
            if (_e145 < 3u) {
                let _e147 = normalizedHeight;
                let _e148 = i_1;
                let _e152 = uboFS.heightSteps[_e148][0u];
                let _e154 = uboFS.heightTransitionRange;
                let _e156 = (_e147 > (_e152 - _e154));
                phi_578_ = _e156;
                if _e156 {
                    let _e157 = normalizedHeight;
                    let _e158 = i_1;
                    let _e162 = uboFS.heightSteps[_e158][0u];
                    let _e164 = uboFS.heightTransitionRange;
                    phi_578_ = (_e157 < (_e162 + _e164));
                }
                let _e168 = phi_578_;
                if _e168 {
                    let _e169 = normalizedHeight;
                    let _e170 = i_1;
                    let _e174 = uboFS.heightSteps[_e170][0u];
                    let _e177 = uboFS.heightTransitionRange;
                    let _e180 = uboFS.heightTransitionRange;
                    ratio = (((_e169 - _e174) + _e177) / (2f * _e180));
                    let _e183 = i_1;
                    param_33 = _e183;
                    let _e184 = inTextureCoord_1;
                    param_34 = _e184;
                    let _e185 = ddx_1;
                    param_35 = _e185;
                    let _e186 = ddy_1;
                    param_36 = _e186;
                    let _e187 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_33), (&param_34), (&param_35), (&param_36));
                    color1_ = _e187;
                    let _e188 = i_1;
                    param_37 = (_e188 + 1u);
                    let _e190 = inTextureCoord_1;
                    param_38 = _e190;
                    let _e191 = ddx_1;
                    param_39 = _e191;
                    let _e192 = ddy_1;
                    param_40 = _e192;
                    let _e193 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_37), (&param_38), (&param_39), (&param_40));
                    color2_ = _e193;
                    let _e194 = color1_;
                    let _e195 = color2_;
                    let _e196 = ratio;
                    textureColor = mix(_e194, _e195, vec4(_e196));
                    let _e199 = i_1;
                    let _e203 = uboFS.material[_e199].shineDamper;
                    shineDamper1_ = _e203;
                    let _e204 = i_1;
                    let _e209 = uboFS.material[(_e204 + 1u)].shineDamper;
                    shineDamper2_ = _e209;
                    let _e210 = shineDamper1_;
                    let _e211 = shineDamper2_;
                    let _e212 = ratio;
                    shineDamper_1 = mix(_e210, _e211, _e212);
                    let _e214 = i_1;
                    let _e218 = uboFS.material[_e214].reflectivity;
                    reflectivity1_ = _e218;
                    let _e219 = i_1;
                    let _e224 = uboFS.material[(_e219 + 1u)].reflectivity;
                    reflectivity2_ = _e224;
                    let _e225 = reflectivity1_;
                    let _e226 = reflectivity2_;
                    let _e227 = ratio;
                    reflectivity_1 = mix(_e225, _e226, _e227);
                    break;
                } else {
                    let _e229 = normalizedHeight;
                    let _e230 = i_1;
                    let _e234 = uboFS.heightSteps[_e230][0u];
                    let _e236 = uboFS.heightTransitionRange;
                    if (_e229 < (_e234 - _e236)) {
                        let _e239 = i_1;
                        param_41 = _e239;
                        let _e240 = inTextureCoord_1;
                        param_42 = _e240;
                        let _e241 = ddx_1;
                        param_43 = _e241;
                        let _e242 = ddy_1;
                        param_44 = _e242;
                        let _e243 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_41), (&param_42), (&param_43), (&param_44));
                        textureColor = _e243;
                        let _e244 = i_1;
                        let _e248 = uboFS.material[_e244].shineDamper;
                        shineDamper_1 = _e248;
                        let _e249 = i_1;
                        let _e253 = uboFS.material[_e249].reflectivity;
                        reflectivity_1 = _e253;
                        break;
                    }
                }
            } else {
                let _e254 = i_1;
                param_45 = _e254;
                let _e255 = inTextureCoord_1;
                param_46 = _e255;
                let _e256 = ddx_1;
                param_47 = _e256;
                let _e257 = ddy_1;
                param_48 = _e257;
                let _e258 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_45), (&param_46), (&param_47), (&param_48));
                textureColor = _e258;
                let _e259 = i_1;
                let _e263 = uboFS.material[_e259].shineDamper;
                shineDamper_1 = _e263;
                let _e264 = i_1;
                let _e268 = uboFS.material[_e264].reflectivity;
                reflectivity_1 = _e268;
                break;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e269 = i_1;
            i_1 = (_e269 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e273 = uboFS.castedByShadows;
    if (_e273 != 0u) {
        let _e276 = uboFS.shadows;
        param_49.cascades[0i].viewProjectionMatrix = _e276.cascades[0].viewProjectionMatrix;
        param_49.cascades[0i].split = _e276.cascades[0].split;
        param_49.cascades[1i].viewProjectionMatrix = _e276.cascades[1].viewProjectionMatrix;
        param_49.cascades[1i].split = _e276.cascades[1].split;
        param_49.cascades[2i].viewProjectionMatrix = _e276.cascades[2].viewProjectionMatrix;
        param_49.cascades[2i].split = _e276.cascades[2].split;
        param_49.cascades[3i].viewProjectionMatrix = _e276.cascades[3].viewProjectionMatrix;
        param_49.cascades[3i].split = _e276.cascades[3].split;
        param_49.enabled = _e276.enabled;
        param_49.useReverseDepth = _e276.useReverseDepth;
        let _e307 = inViewPosition_1;
        param_50 = _e307;
        let _e308 = inWorldPosition_1;
        param_51 = _e308;
        param_52 = 0.02f;
        let _e309 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_49), (&param_50), (&param_51), (&param_52));
        shadow_4 = _e309;
    }
    let _e310 = inNormal_1;
    unitNormal = normalize(_e310);
    let _e312 = inToCameraVector_1;
    unitToCameraVector = normalize(_e312);
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_2 = 0u;
    loop {
        let _e314 = i_2;
        let _e317 = uboFS.lightning.realCountOfLights;
        if (_e314 < _e317) {
            let _e319 = i_2;
            let _e323 = uboFS.lightning.lights[_e319];
            light.position = _e323.position;
            light.color = _e323.color;
            light.attenuation = _e323.attenuation;
            let _e330 = i_2;
            let _e332 = inToLightVector_arr[_e330];
            toLightVector_3 = _e332;
            let _e333 = toLightVector_3;
            unitToLightVector = normalize(_e333);
            let _e336 = light.attenuation;
            param_53 = _e336.xyz;
            let _e338 = toLightVector_3;
            param_54 = _e338;
            let _e339 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_53), (&param_54));
            attenuationFactor_3 = _e339;
            let _e340 = unitNormal;
            param_55 = _e340;
            let _e341 = unitToLightVector;
            param_56 = _e341;
            let _e343 = light.color;
            param_57 = _e343.xyz;
            let _e345 = attenuationFactor_3;
            param_58 = _e345;
            let _e346 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_55), (&param_56), (&param_57), (&param_58));
            let _e347 = totalDiffuse;
            totalDiffuse = (_e347 + _e346);
            let _e349 = unitNormal;
            param_59 = _e349;
            let _e350 = unitToLightVector;
            param_60 = _e350;
            let _e351 = unitToCameraVector;
            param_61 = _e351;
            let _e353 = light.color;
            param_62 = _e353.xyz;
            let _e355 = attenuationFactor_3;
            param_63 = _e355;
            let _e356 = shineDamper_1;
            param_64 = _e356;
            let _e357 = reflectivity_1;
            param_65 = _e357;
            let _e358 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_59), (&param_60), (&param_61), (&param_62), (&param_63), (&param_64), (&param_65));
            let _e359 = totalSpecular;
            totalSpecular = (_e359 + _e358);
            continue;
        } else {
            break;
        }
        continuing {
            let _e361 = i_2;
            i_2 = (_e361 + bitcast<u32>(1i));
        }
    }
    let _e364 = totalDiffuse;
    let _e365 = shadow_4;
    let _e369 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e364 * _e365) + vec3(_e369));
    let _e372 = totalSpecular;
    let _e373 = shadow_4;
    totalSpecular = (_e372 * _e373);
    let _e375 = totalDiffuse;
    let _e380 = textureColor;
    let _e382 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e375.x, _e375.y, _e375.z, 1f) * _e380) + vec4<f32>(_e382.x, _e382.y, _e382.z, 0f));
    let _e389 = uboFS.fogColor;
    let _e390 = _e389.xyz;
    let _e395 = baseResultColor;
    let _e396 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e390.x, _e390.y, _e390.z, 1f), _e395, vec4(_e396));
    let _e400 = uboFS.selected;
    if (_e400 != 0u) {
        let _e402 = resultColor;
        let _e404 = uboFS.selectedColor;
        resultColor = mix(_e402, _e404, vec4(0.5f));
    }
    let _e407 = resultColor;
    outColor = _e407;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(6) inToLightVector0_: vec3<f32>, @location(7) inToLightVector1_: vec3<f32>, @location(8) inToLightVector2_: vec3<f32>, @location(9) inToLightVector3_: vec3<f32>, @location(10) inClipDistance: f32, @location(2) inWorldPosition: vec3<f32>, @location(0) inTextureCoord: vec2<f32>, @location(3) inViewPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(4) inToCameraVector: vec3<f32>, @location(5) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVector0_1 = inToLightVector0_;
    inToLightVector1_1 = inToLightVector1_;
    inToLightVector2_1 = inToLightVector2_;
    inToLightVector3_1 = inToLightVector3_;
    inClipDistance_1 = inClipDistance;
    inWorldPosition_1 = inWorldPosition;
    inTextureCoord_1 = inTextureCoord;
    inViewPosition_1 = inViewPosition;
    inNormal_1 = inNormal;
    inToCameraVector_1 = inToCameraVector;
    inVisibility_1 = inVisibility;
    main_1();
    let _e25 = outColor;
    return _e25;
}
