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
var colorTextures: texture_2d_array<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
var<private> inToLightVector0_1: vec3<f32>;
var<private> inToLightVector1_1: vec3<f32>;
var<private> inToLightVector2_1: vec3<f32>;
var<private> inToLightVector3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(4) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(5) 
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

    let _e66 = (*toLightVector);
    lightDirection = -(_e66);
    let _e68 = lightDirection;
    let _e69 = (*normal);
    reflectedLightDirection = reflect(_e68, _e69);
    if true {
        let _e71 = (*toLightVector);
        let _e72 = (*toCameraVector);
        halfwayDir = normalize((_e71 + _e72));
        let _e75 = (*normal);
        let _e76 = halfwayDir;
        specularFactor = max(dot(_e75, _e76), 0f);
        let _e79 = (*shineDamper);
        shinePower = (_e79 * 2.4f);
    } else {
        let _e81 = reflectedLightDirection;
        let _e82 = (*toCameraVector);
        specularFactor = max(dot(_e81, _e82), 0f);
        let _e85 = (*shineDamper);
        shinePower = _e85;
    }
    let _e86 = specularFactor;
    let _e87 = shinePower;
    dampedFactor = pow(_e86, _e87);
    let _e89 = dampedFactor;
    let _e90 = (*reflectivity);
    let _e92 = (*lightColor);
    let _e94 = (*attenuationFactor);
    return ((_e92 * (_e89 * _e90)) / vec3(_e94));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e59 = (*normal_1);
    let _e60 = (*toLightVector_1);
    nDotL = dot(_e59, _e60);
    let _e62 = nDotL;
    brightness = max(_e62, 0f);
    let _e64 = brightness;
    let _e65 = (*lightColor_1);
    let _e67 = (*attenuationFactor_1);
    return ((_e65 * _e64) / vec3(_e67));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e57 = (*toLightVector_2);
    toLightDistance = length(_e57);
    let _e60 = (*attenuation)[0u];
    let _e62 = (*attenuation)[1u];
    let _e63 = toLightDistance;
    let _e67 = (*attenuation)[2u];
    let _e68 = toLightDistance;
    let _e70 = toLightDistance;
    attenuationFactor_2 = ((_e60 + (_e62 * _e63)) + ((_e67 * _e68) * _e70));
    let _e73 = attenuationFactor_2;
    return _e73;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_104_: bool;

    shadow = 1f;
    let _e63 = (*shadowCoord)[2u];
    let _e64 = (_e63 >= 0f);
    phi_104_ = _e64;
    if _e64 {
        let _e66 = (*shadowCoord)[2u];
        phi_104_ = (_e66 <= 1f);
    }
    let _e69 = phi_104_;
    if _e69 {
        let _e70 = (*shadowCoord);
        let _e72 = (*shadowCoordOffset);
        let _e73 = (_e70.xy + _e72);
        let _e74 = (*cascadeIndex);
        let _e78 = vec3<f32>(_e73.x, _e73.y, f32(_e74));
        let _e84 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e78.x, _e78.y), i32(_e78.z), 0f);
        depth = _e84.x;
        let _e86 = (*useReverseDepth);
        if (_e86 != 0u) {
            let _e88 = depth;
            let _e90 = (*shadowCoord)[2u];
            let _e91 = (*depthBias);
            if (_e88 > (_e90 + _e91)) {
                shadow = 0.2f;
            }
        } else {
            let _e94 = depth;
            let _e96 = (*shadowCoord)[2u];
            let _e97 = (*depthBias);
            if (_e94 < (_e96 - _e97)) {
                shadow = 0.2f;
            }
        }
    }
    let _e100 = shadow;
    return _e100;
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

    let _e83 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e83).xy);
    let _e87 = textureDim;
    texelSize = (vec2(1f) / _e87);
    shadow_1 = 0f;
    let _e90 = gl_FragCoord_1;
    offset = (fract((_e90.xy * 0.5f)) + vec2(0.25f));
    let _e97 = offset[1u];
    if (_e97 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e100 = texelSize;
    let _e101 = offset;
    let _e104 = (*shadowCoord_1);
    param = _e104;
    param_1 = (_e100 * (_e101 + vec2<f32>(-1.5f, 0.5f)));
    let _e105 = (*cascadeIndex_1);
    param_2 = _e105;
    let _e106 = (*depthBias_1);
    param_3 = _e106;
    let _e107 = (*useReverseDepth_1);
    param_4 = _e107;
    let _e108 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e109 = texelSize;
    let _e110 = offset;
    let _e113 = (*shadowCoord_1);
    param_5 = _e113;
    param_6 = (_e109 * (_e110 + vec2<f32>(0.5f, 0.5f)));
    let _e114 = (*cascadeIndex_1);
    param_7 = _e114;
    let _e115 = (*depthBias_1);
    param_8 = _e115;
    let _e116 = (*useReverseDepth_1);
    param_9 = _e116;
    let _e117 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e119 = texelSize;
    let _e120 = offset;
    let _e123 = (*shadowCoord_1);
    param_10 = _e123;
    param_11 = (_e119 * (_e120 + vec2<f32>(-1.5f, -1.5f)));
    let _e124 = (*cascadeIndex_1);
    param_12 = _e124;
    let _e125 = (*depthBias_1);
    param_13 = _e125;
    let _e126 = (*useReverseDepth_1);
    param_14 = _e126;
    let _e127 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e129 = texelSize;
    let _e130 = offset;
    let _e133 = (*shadowCoord_1);
    param_15 = _e133;
    param_16 = (_e129 * (_e130 + vec2<f32>(0.5f, -1.5f)));
    let _e134 = (*cascadeIndex_1);
    param_17 = _e134;
    let _e135 = (*depthBias_1);
    param_18 = _e135;
    let _e136 = (*useReverseDepth_1);
    param_19 = _e136;
    let _e137 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e108 + _e117) + _e127) + _e137) * 0.25f);
    let _e140 = shadow_1;
    return _e140;
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
        let _e69 = (*shadowCoord_2);
        param_20 = _e69;
        let _e70 = (*cascadeIndex_2);
        param_21 = _e70;
        let _e71 = (*depthBias_2);
        param_22 = _e71;
        let _e72 = (*useReverseDepth_2);
        param_23 = _e72;
        let _e73 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e73;
    } else {
        let _e74 = (*shadowCoord_2);
        param_24 = _e74;
        param_25 = vec2<f32>(0f, 0f);
        let _e75 = (*cascadeIndex_2);
        param_26 = _e75;
        let _e76 = (*depthBias_2);
        param_27 = _e76;
        let _e77 = (*useReverseDepth_2);
        param_28 = _e77;
        let _e78 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e78;
    }
    let _e79 = shadow_2;
    return _e79;
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
    let _e69 = (*depthBias_3);
    bias = _e69;
    let _e71 = (*shadows).enabled;
    if (_e71 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e73 = i;
            if (_e73 < 3u) {
                let _e76 = (*shadows).useReverseDepth;
                if (_e76 != 0u) {
                    let _e79 = (*viewPosition)[2u];
                    let _e80 = i;
                    let _e85 = (*shadows).cascades[_e80].split[0u];
                    if (_e79 > _e85) {
                        let _e87 = i;
                        cascadeIndex_3 = (_e87 + 1u);
                        let _e89 = bias;
                        bias = (_e89 / 1.5f);
                    }
                } else {
                    let _e92 = (*viewPosition)[2u];
                    let _e93 = i;
                    let _e98 = (*shadows).cascades[_e93].split[0u];
                    if (_e92 < _e98) {
                        let _e100 = i;
                        cascadeIndex_3 = (_e100 + 1u);
                        let _e102 = bias;
                        bias = (_e102 / 1.5f);
                    }
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e104 = i;
                i = (_e104 + bitcast<u32>(1i));
            }
        }
        let _e107 = cascadeIndex_3;
        let _e111 = (*shadows).cascades[_e107].viewProjectionMatrix;
        let _e112 = (*worldPosition);
        shadowCoord_3 = (_e111 * vec4<f32>(_e112.x, _e112.y, _e112.z, 1f));
        let _e118 = shadowCoord_3;
        let _e120 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e118 / vec4(_e120));
        let _e123 = normalizedShadowCoord;
        param_29 = _e123;
        let _e124 = cascadeIndex_3;
        param_30 = _e124;
        let _e125 = bias;
        param_31 = _e125;
        let _e127 = (*shadows).useReverseDepth;
        param_32 = _e127;
        let _e128 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e128;
    }
    let _e129 = shadow_3;
    return _e129;
}

fn sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e57 = (*uv);
    let _e58 = (*idx);
    let _e62 = vec3<f32>(_e57.x, _e57.y, f32(_e58));
    let _e63 = (*ddx);
    let _e64 = (*ddy);
    let _e70 = textureSampleGrad(colorTextures, colorSampler, vec2<f32>(_e62.x, _e62.y), i32(_e62.z), _e63, _e64);
    return _e70;
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
    var phi_556_: bool;

    let _e114 = inToLightVector0_1;
    let _e115 = inToLightVector1_1;
    let _e116 = inToLightVector2_1;
    let _e117 = inToLightVector3_1;
    inToLightVector_arr = array<vec3<f32>, 4>(_e114, _e115, _e116, _e117);
    let _e119 = inClipDistance_1;
    if (_e119 < 0f) {
        discard;
    }
    let _e122 = uboFS.maxHeight;
    let _e125 = uboFS.minHeight;
    heightRange = (abs(_e122) + abs(_e125));
    let _e129 = inWorldPosition_1[1u];
    let _e131 = uboFS.minHeight;
    let _e134 = heightRange;
    normalizedHeight = ((_e129 + abs(_e131)) / _e134);
    textureColor = vec4<f32>(1f, 1f, 0f, 1f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e136 = inTextureCoord_1;
    let _e137 = dpdx(_e136);
    ddx_1 = _e137;
    let _e138 = inTextureCoord_1;
    let _e139 = dpdy(_e138);
    ddy_1 = _e139;
    i_1 = 0u;
    loop {
        let _e140 = i_1;
        if (_e140 < 4u) {
            let _e142 = i_1;
            if (_e142 < 3u) {
                let _e144 = normalizedHeight;
                let _e145 = i_1;
                let _e149 = uboFS.heightSteps[_e145][0u];
                let _e151 = uboFS.heightTransitionRange;
                let _e153 = (_e144 > (_e149 - _e151));
                phi_556_ = _e153;
                if _e153 {
                    let _e154 = normalizedHeight;
                    let _e155 = i_1;
                    let _e159 = uboFS.heightSteps[_e155][0u];
                    let _e161 = uboFS.heightTransitionRange;
                    phi_556_ = (_e154 < (_e159 + _e161));
                }
                let _e165 = phi_556_;
                if _e165 {
                    let _e166 = normalizedHeight;
                    let _e167 = i_1;
                    let _e171 = uboFS.heightSteps[_e167][0u];
                    let _e174 = uboFS.heightTransitionRange;
                    let _e177 = uboFS.heightTransitionRange;
                    ratio = (((_e166 - _e171) + _e174) / (2f * _e177));
                    let _e180 = i_1;
                    param_33 = _e180;
                    let _e181 = inTextureCoord_1;
                    param_34 = _e181;
                    let _e182 = ddx_1;
                    param_35 = _e182;
                    let _e183 = ddy_1;
                    param_36 = _e183;
                    let _e184 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_33), (&param_34), (&param_35), (&param_36));
                    color1_ = _e184;
                    let _e185 = i_1;
                    param_37 = (_e185 + 1u);
                    let _e187 = inTextureCoord_1;
                    param_38 = _e187;
                    let _e188 = ddx_1;
                    param_39 = _e188;
                    let _e189 = ddy_1;
                    param_40 = _e189;
                    let _e190 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_37), (&param_38), (&param_39), (&param_40));
                    color2_ = _e190;
                    let _e191 = color1_;
                    let _e192 = color2_;
                    let _e193 = ratio;
                    textureColor = mix(_e191, _e192, vec4(_e193));
                    let _e196 = i_1;
                    let _e200 = uboFS.material[_e196].shineDamper;
                    shineDamper1_ = _e200;
                    let _e201 = i_1;
                    let _e206 = uboFS.material[(_e201 + 1u)].shineDamper;
                    shineDamper2_ = _e206;
                    let _e207 = shineDamper1_;
                    let _e208 = shineDamper2_;
                    let _e209 = ratio;
                    shineDamper_1 = mix(_e207, _e208, _e209);
                    let _e211 = i_1;
                    let _e215 = uboFS.material[_e211].reflectivity;
                    reflectivity1_ = _e215;
                    let _e216 = i_1;
                    let _e221 = uboFS.material[(_e216 + 1u)].reflectivity;
                    reflectivity2_ = _e221;
                    let _e222 = reflectivity1_;
                    let _e223 = reflectivity2_;
                    let _e224 = ratio;
                    reflectivity_1 = mix(_e222, _e223, _e224);
                    break;
                } else {
                    let _e226 = normalizedHeight;
                    let _e227 = i_1;
                    let _e231 = uboFS.heightSteps[_e227][0u];
                    let _e233 = uboFS.heightTransitionRange;
                    if (_e226 < (_e231 - _e233)) {
                        let _e236 = i_1;
                        param_41 = _e236;
                        let _e237 = inTextureCoord_1;
                        param_42 = _e237;
                        let _e238 = ddx_1;
                        param_43 = _e238;
                        let _e239 = ddy_1;
                        param_44 = _e239;
                        let _e240 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_41), (&param_42), (&param_43), (&param_44));
                        textureColor = _e240;
                        let _e241 = i_1;
                        let _e245 = uboFS.material[_e241].shineDamper;
                        shineDamper_1 = _e245;
                        let _e246 = i_1;
                        let _e250 = uboFS.material[_e246].reflectivity;
                        reflectivity_1 = _e250;
                        break;
                    }
                }
            } else {
                let _e251 = i_1;
                param_45 = _e251;
                let _e252 = inTextureCoord_1;
                param_46 = _e252;
                let _e253 = ddx_1;
                param_47 = _e253;
                let _e254 = ddy_1;
                param_48 = _e254;
                let _e255 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_45), (&param_46), (&param_47), (&param_48));
                textureColor = _e255;
                let _e256 = i_1;
                let _e260 = uboFS.material[_e256].shineDamper;
                shineDamper_1 = _e260;
                let _e261 = i_1;
                let _e265 = uboFS.material[_e261].reflectivity;
                reflectivity_1 = _e265;
                break;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e266 = i_1;
            i_1 = (_e266 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e270 = uboFS.castedByShadows;
    if (_e270 != 0u) {
        let _e273 = uboFS.shadows;
        param_49.cascades[0i].viewProjectionMatrix = _e273.cascades[0].viewProjectionMatrix;
        param_49.cascades[0i].split = _e273.cascades[0].split;
        param_49.cascades[1i].viewProjectionMatrix = _e273.cascades[1].viewProjectionMatrix;
        param_49.cascades[1i].split = _e273.cascades[1].split;
        param_49.cascades[2i].viewProjectionMatrix = _e273.cascades[2].viewProjectionMatrix;
        param_49.cascades[2i].split = _e273.cascades[2].split;
        param_49.cascades[3i].viewProjectionMatrix = _e273.cascades[3].viewProjectionMatrix;
        param_49.cascades[3i].split = _e273.cascades[3].split;
        param_49.enabled = _e273.enabled;
        param_49.useReverseDepth = _e273.useReverseDepth;
        let _e304 = inViewPosition_1;
        param_50 = _e304;
        let _e305 = inWorldPosition_1;
        param_51 = _e305;
        param_52 = 0.005f;
        let _e306 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_49), (&param_50), (&param_51), (&param_52));
        shadow_4 = _e306;
    }
    let _e307 = inNormal_1;
    unitNormal = normalize(_e307);
    let _e309 = inToCameraVector_1;
    unitToCameraVector = normalize(_e309);
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_2 = 0u;
    loop {
        let _e311 = i_2;
        let _e314 = uboFS.lightning.realCountOfLights;
        if (_e311 < _e314) {
            let _e316 = i_2;
            let _e320 = uboFS.lightning.lights[_e316];
            light.position = _e320.position;
            light.color = _e320.color;
            light.attenuation = _e320.attenuation;
            let _e327 = i_2;
            let _e329 = inToLightVector_arr[_e327];
            toLightVector_3 = _e329;
            let _e330 = toLightVector_3;
            unitToLightVector = normalize(_e330);
            let _e333 = light.attenuation;
            param_53 = _e333.xyz;
            let _e335 = toLightVector_3;
            param_54 = _e335;
            let _e336 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_53), (&param_54));
            attenuationFactor_3 = _e336;
            let _e337 = unitNormal;
            param_55 = _e337;
            let _e338 = unitToLightVector;
            param_56 = _e338;
            let _e340 = light.color;
            param_57 = _e340.xyz;
            let _e342 = attenuationFactor_3;
            param_58 = _e342;
            let _e343 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_55), (&param_56), (&param_57), (&param_58));
            let _e344 = totalDiffuse;
            totalDiffuse = (_e344 + _e343);
            let _e346 = unitNormal;
            param_59 = _e346;
            let _e347 = unitToLightVector;
            param_60 = _e347;
            let _e348 = unitToCameraVector;
            param_61 = _e348;
            let _e350 = light.color;
            param_62 = _e350.xyz;
            let _e352 = attenuationFactor_3;
            param_63 = _e352;
            let _e353 = shineDamper_1;
            param_64 = _e353;
            let _e354 = reflectivity_1;
            param_65 = _e354;
            let _e355 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_59), (&param_60), (&param_61), (&param_62), (&param_63), (&param_64), (&param_65));
            let _e356 = totalSpecular;
            totalSpecular = (_e356 + _e355);
            continue;
        } else {
            break;
        }
        continuing {
            let _e358 = i_2;
            i_2 = (_e358 + bitcast<u32>(1i));
        }
    }
    let _e361 = totalDiffuse;
    let _e362 = shadow_4;
    let _e366 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e361 * _e362) + vec3(_e366));
    let _e369 = totalSpecular;
    let _e370 = shadow_4;
    totalSpecular = (_e369 * _e370);
    let _e372 = totalDiffuse;
    let _e377 = textureColor;
    let _e379 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e372.x, _e372.y, _e372.z, 1f) * _e377) + vec4<f32>(_e379.x, _e379.y, _e379.z, 0f));
    let _e386 = uboFS.fogColor;
    let _e387 = _e386.xyz;
    let _e392 = baseResultColor;
    let _e393 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e387.x, _e387.y, _e387.z, 1f), _e392, vec4(_e393));
    let _e397 = uboFS.selected;
    if (_e397 != 0u) {
        let _e399 = resultColor;
        let _e401 = uboFS.selectedColor;
        resultColor = mix(_e399, _e401, vec4(0.5f));
    }
    let _e404 = resultColor;
    outColor = _e404;
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
