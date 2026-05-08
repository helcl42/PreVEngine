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
    material: Material,
    fogColor: vec4<f32>,
    selectedColor: vec4<f32>,
    selected: u32,
    castedByShadows: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
var<private> inToLightVector0_1: vec3<f32>;
var<private> inToLightVector1_1: vec3<f32>;
var<private> inToLightVector2_1: vec3<f32>;
var<private> inToLightVector3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
@group(0) @binding(2) 
var colorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
@group(0) @binding(4) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(5) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inWorldPosition_1: vec3<f32>;
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

    let _e60 = (*toLightVector);
    lightDirection = -(_e60);
    let _e62 = lightDirection;
    let _e63 = (*normal);
    reflectedLightDirection = reflect(_e62, _e63);
    if true {
        let _e65 = (*toLightVector);
        let _e66 = (*toCameraVector);
        halfwayDir = normalize((_e65 + _e66));
        let _e69 = (*normal);
        let _e70 = halfwayDir;
        specularFactor = max(dot(_e69, _e70), 0f);
        let _e73 = (*shineDamper);
        shinePower = (_e73 * 2.4f);
    } else {
        let _e75 = reflectedLightDirection;
        let _e76 = (*toCameraVector);
        specularFactor = max(dot(_e75, _e76), 0f);
        let _e79 = (*shineDamper);
        shinePower = _e79;
    }
    let _e80 = specularFactor;
    let _e81 = shinePower;
    dampedFactor = pow(_e80, _e81);
    let _e83 = dampedFactor;
    let _e84 = (*reflectivity);
    let _e86 = (*lightColor);
    let _e88 = (*attenuationFactor);
    return ((_e86 * (_e83 * _e84)) / vec3(_e88));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e53 = (*normal_1);
    let _e54 = (*toLightVector_1);
    nDotL = dot(_e53, _e54);
    let _e56 = nDotL;
    brightness = max(_e56, 0f);
    let _e58 = brightness;
    let _e59 = (*lightColor_1);
    let _e61 = (*attenuationFactor_1);
    return ((_e59 * _e58) / vec3(_e61));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e51 = (*toLightVector_2);
    toLightDistance = length(_e51);
    let _e54 = (*attenuation)[0u];
    let _e56 = (*attenuation)[1u];
    let _e57 = toLightDistance;
    let _e61 = (*attenuation)[2u];
    let _e62 = toLightDistance;
    let _e64 = toLightDistance;
    attenuationFactor_2 = ((_e54 + (_e56 * _e57)) + ((_e61 * _e62) * _e64));
    let _e67 = attenuationFactor_2;
    return _e67;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_97_: bool;

    shadow = 1f;
    let _e57 = (*shadowCoord)[2u];
    let _e58 = (_e57 >= 0f);
    phi_97_ = _e58;
    if _e58 {
        let _e60 = (*shadowCoord)[2u];
        phi_97_ = (_e60 <= 1f);
    }
    let _e63 = phi_97_;
    if _e63 {
        let _e64 = (*shadowCoord);
        let _e66 = (*shadowCoordOffset);
        let _e67 = (_e64.xy + _e66);
        let _e68 = (*cascadeIndex);
        let _e72 = vec3<f32>(_e67.x, _e67.y, f32(_e68));
        let _e78 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e72.x, _e72.y), i32(_e72.z), 0f);
        depth = _e78.x;
        let _e80 = (*useReverseDepth);
        if (_e80 != 0u) {
            let _e82 = depth;
            let _e84 = (*shadowCoord)[2u];
            let _e85 = (*depthBias);
            if (_e82 > (_e84 + _e85)) {
                shadow = 0.2f;
            }
        } else {
            let _e88 = depth;
            let _e90 = (*shadowCoord)[2u];
            let _e91 = (*depthBias);
            if (_e88 < (_e90 - _e91)) {
                shadow = 0.2f;
            }
        }
    }
    let _e94 = shadow;
    return _e94;
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

    let _e77 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e77).xy);
    let _e81 = textureDim;
    texelSize = (vec2(1f) / _e81);
    shadow_1 = 0f;
    let _e84 = gl_FragCoord_1;
    offset = (fract((_e84.xy * 0.5f)) + vec2(0.25f));
    let _e91 = offset[1u];
    if (_e91 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e94 = texelSize;
    let _e95 = offset;
    let _e98 = (*shadowCoord_1);
    param = _e98;
    param_1 = (_e94 * (_e95 + vec2<f32>(-1.5f, 0.5f)));
    let _e99 = (*cascadeIndex_1);
    param_2 = _e99;
    let _e100 = (*depthBias_1);
    param_3 = _e100;
    let _e101 = (*useReverseDepth_1);
    param_4 = _e101;
    let _e102 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e103 = texelSize;
    let _e104 = offset;
    let _e107 = (*shadowCoord_1);
    param_5 = _e107;
    param_6 = (_e103 * (_e104 + vec2<f32>(0.5f, 0.5f)));
    let _e108 = (*cascadeIndex_1);
    param_7 = _e108;
    let _e109 = (*depthBias_1);
    param_8 = _e109;
    let _e110 = (*useReverseDepth_1);
    param_9 = _e110;
    let _e111 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e113 = texelSize;
    let _e114 = offset;
    let _e117 = (*shadowCoord_1);
    param_10 = _e117;
    param_11 = (_e113 * (_e114 + vec2<f32>(-1.5f, -1.5f)));
    let _e118 = (*cascadeIndex_1);
    param_12 = _e118;
    let _e119 = (*depthBias_1);
    param_13 = _e119;
    let _e120 = (*useReverseDepth_1);
    param_14 = _e120;
    let _e121 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e123 = texelSize;
    let _e124 = offset;
    let _e127 = (*shadowCoord_1);
    param_15 = _e127;
    param_16 = (_e123 * (_e124 + vec2<f32>(0.5f, -1.5f)));
    let _e128 = (*cascadeIndex_1);
    param_17 = _e128;
    let _e129 = (*depthBias_1);
    param_18 = _e129;
    let _e130 = (*useReverseDepth_1);
    param_19 = _e130;
    let _e131 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e102 + _e111) + _e121) + _e131) * 0.25f);
    let _e134 = shadow_1;
    return _e134;
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
        let _e63 = (*shadowCoord_2);
        param_20 = _e63;
        let _e64 = (*cascadeIndex_2);
        param_21 = _e64;
        let _e65 = (*depthBias_2);
        param_22 = _e65;
        let _e66 = (*useReverseDepth_2);
        param_23 = _e66;
        let _e67 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e67;
    } else {
        let _e68 = (*shadowCoord_2);
        param_24 = _e68;
        param_25 = vec2<f32>(0f, 0f);
        let _e69 = (*cascadeIndex_2);
        param_26 = _e69;
        let _e70 = (*depthBias_2);
        param_27 = _e70;
        let _e71 = (*useReverseDepth_2);
        param_28 = _e71;
        let _e72 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e72;
    }
    let _e73 = shadow_2;
    return _e73;
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
    let _e63 = (*depthBias_3);
    bias = _e63;
    let _e65 = (*shadows).enabled;
    if (_e65 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e67 = i;
            if (_e67 < 3u) {
                let _e70 = (*viewPosition)[2u];
                let _e71 = i;
                let _e76 = (*shadows).cascades[_e71].split[0u];
                if (_e70 < _e76) {
                    let _e78 = i;
                    cascadeIndex_3 = (_e78 + 1u);
                    let _e80 = bias;
                    bias = (_e80 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e82 = i;
                i = (_e82 + bitcast<u32>(1i));
            }
        }
        let _e85 = cascadeIndex_3;
        let _e89 = (*shadows).cascades[_e85].viewProjectionMatrix;
        let _e90 = (*worldPosition);
        shadowCoord_3 = (_e89 * vec4<f32>(_e90.x, _e90.y, _e90.z, 1f));
        let _e96 = shadowCoord_3;
        let _e98 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e96 / vec4(_e98));
        let _e101 = normalizedShadowCoord;
        param_29 = _e101;
        let _e102 = cascadeIndex_3;
        param_30 = _e102;
        let _e103 = bias;
        param_31 = _e103;
        let _e105 = (*shadows).useReverseDepth;
        param_32 = _e105;
        let _e106 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e106;
    }
    let _e107 = shadow_3;
    return _e107;
}

fn main_1() {
    var inToLightVector_arr: array<vec3<f32>, 4>;
    var textureColor: vec4<f32>;
    var shadow_4: f32;
    var param_33: Shadows;
    var param_34: vec3<f32>;
    var param_35: vec3<f32>;
    var param_36: f32;
    var unitNormal: vec3<f32>;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_1: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_37: vec3<f32>;
    var param_38: vec3<f32>;
    var param_39: vec3<f32>;
    var param_40: vec3<f32>;
    var param_41: vec3<f32>;
    var param_42: f32;
    var param_43: vec3<f32>;
    var param_44: vec3<f32>;
    var param_45: vec3<f32>;
    var param_46: vec3<f32>;
    var param_47: f32;
    var param_48: f32;
    var param_49: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;

    let _e78 = inToLightVector0_1;
    let _e79 = inToLightVector1_1;
    let _e80 = inToLightVector2_1;
    let _e81 = inToLightVector3_1;
    inToLightVector_arr = array<vec3<f32>, 4>(_e78, _e79, _e80, _e81);
    let _e83 = inClipDistance_1;
    if (_e83 < 0f) {
        discard;
    }
    let _e85 = inTextureCoord_1;
    let _e86 = textureSample(colorTexture, colorSampler, _e85);
    textureColor = _e86;
    shadow_4 = 1f;
    let _e88 = uboFS.castedByShadows;
    if (_e88 != 0u) {
        let _e91 = uboFS.shadows;
        param_33.cascades[0i].viewProjectionMatrix = _e91.cascades[0].viewProjectionMatrix;
        param_33.cascades[0i].split = _e91.cascades[0].split;
        param_33.cascades[1i].viewProjectionMatrix = _e91.cascades[1].viewProjectionMatrix;
        param_33.cascades[1i].split = _e91.cascades[1].split;
        param_33.cascades[2i].viewProjectionMatrix = _e91.cascades[2].viewProjectionMatrix;
        param_33.cascades[2i].split = _e91.cascades[2].split;
        param_33.cascades[3i].viewProjectionMatrix = _e91.cascades[3].viewProjectionMatrix;
        param_33.cascades[3i].split = _e91.cascades[3].split;
        param_33.enabled = _e91.enabled;
        param_33.useReverseDepth = _e91.useReverseDepth;
        let _e122 = inViewPosition_1;
        param_34 = _e122;
        let _e123 = inWorldPosition_1;
        param_35 = _e123;
        param_36 = 0.02f;
        let _e124 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_33), (&param_34), (&param_35), (&param_36));
        shadow_4 = _e124;
    }
    let _e125 = inNormal_1;
    unitNormal = normalize(_e125);
    let _e127 = inToCameraVector_1;
    unitToCameraVector = normalize(_e127);
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_1 = 0u;
    loop {
        let _e129 = i_1;
        let _e132 = uboFS.lightning.realCountOfLights;
        if (_e129 < _e132) {
            let _e134 = i_1;
            let _e138 = uboFS.lightning.lights[_e134];
            light.position = _e138.position;
            light.color = _e138.color;
            light.attenuation = _e138.attenuation;
            let _e145 = i_1;
            let _e147 = inToLightVector_arr[_e145];
            toLightVector_3 = _e147;
            let _e148 = toLightVector_3;
            unitToLightVector = normalize(_e148);
            let _e151 = light.attenuation;
            param_37 = _e151.xyz;
            let _e153 = toLightVector_3;
            param_38 = _e153;
            let _e154 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_37), (&param_38));
            attenuationFactor_3 = _e154;
            let _e155 = unitNormal;
            param_39 = _e155;
            let _e156 = unitToLightVector;
            param_40 = _e156;
            let _e158 = light.color;
            param_41 = _e158.xyz;
            let _e160 = attenuationFactor_3;
            param_42 = _e160;
            let _e161 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_39), (&param_40), (&param_41), (&param_42));
            let _e162 = totalDiffuse;
            totalDiffuse = (_e162 + _e161);
            let _e164 = unitNormal;
            param_43 = _e164;
            let _e165 = unitToLightVector;
            param_44 = _e165;
            let _e166 = unitToCameraVector;
            param_45 = _e166;
            let _e168 = light.color;
            param_46 = _e168.xyz;
            let _e170 = attenuationFactor_3;
            param_47 = _e170;
            let _e173 = uboFS.material.shineDamper;
            param_48 = _e173;
            let _e176 = uboFS.material.reflectivity;
            param_49 = _e176;
            let _e177 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_43), (&param_44), (&param_45), (&param_46), (&param_47), (&param_48), (&param_49));
            let _e178 = totalSpecular;
            totalSpecular = (_e178 + _e177);
            continue;
        } else {
            break;
        }
        continuing {
            let _e180 = i_1;
            i_1 = (_e180 + bitcast<u32>(1i));
        }
    }
    let _e183 = totalDiffuse;
    let _e184 = shadow_4;
    let _e188 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e183 * _e184) + vec3(_e188));
    let _e191 = totalSpecular;
    let _e192 = shadow_4;
    totalSpecular = (_e191 * _e192);
    let _e194 = totalDiffuse;
    let _e199 = textureColor;
    let _e201 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e194.x, _e194.y, _e194.z, 1f) * _e199) + vec4<f32>(_e201.x, _e201.y, _e201.z, 0f));
    let _e208 = uboFS.fogColor;
    let _e209 = _e208.xyz;
    let _e214 = baseResultColor;
    let _e215 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e209.x, _e209.y, _e209.z, 1f), _e214, vec4(_e215));
    let _e219 = uboFS.selected;
    if (_e219 != 0u) {
        let _e221 = resultColor;
        let _e223 = uboFS.selectedColor;
        resultColor = mix(_e221, _e223, vec4(0.5f));
    }
    let _e226 = resultColor;
    outColor = _e226;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(6) inToLightVector0_: vec3<f32>, @location(7) inToLightVector1_: vec3<f32>, @location(8) inToLightVector2_: vec3<f32>, @location(9) inToLightVector3_: vec3<f32>, @location(10) inClipDistance: f32, @location(0) inTextureCoord: vec2<f32>, @location(3) inViewPosition: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(4) inToCameraVector: vec3<f32>, @location(5) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVector0_1 = inToLightVector0_;
    inToLightVector1_1 = inToLightVector1_;
    inToLightVector2_1 = inToLightVector2_;
    inToLightVector3_1 = inToLightVector3_;
    inClipDistance_1 = inClipDistance;
    inTextureCoord_1 = inTextureCoord;
    inViewPosition_1 = inViewPosition;
    inWorldPosition_1 = inWorldPosition;
    inNormal_1 = inNormal;
    inToCameraVector_1 = inToCameraVector;
    inVisibility_1 = inVisibility;
    main_1();
    let _e25 = outColor;
    return _e25;
}
