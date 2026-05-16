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
    hasNormalMap: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
@group(0) @binding(2) 
var colorTextures: texture_2d_array<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
@group(0) @binding(4) 
var normalTextures: texture_2d_array<f32>;
@group(0) @binding(5) 
var normalSampler: sampler;
var<private> inToLightVectorTangentSpace0_1: vec3<f32>;
var<private> inToLightVectorTangentSpace1_1: vec3<f32>;
var<private> inToLightVectorTangentSpace2_1: vec3<f32>;
var<private> inToLightVectorTangentSpace3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
var<private> inToCameraVectorTangentSpace_1: vec3<f32>;
var<private> inPositionTangentSpace_1: vec3<f32>;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
@group(0) @binding(6) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(7) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e71 = (*toLightVector);
    lightDirection = -(_e71);
    let _e73 = lightDirection;
    let _e74 = (*normal);
    reflectedLightDirection = reflect(_e73, _e74);
    if true {
        let _e76 = (*toLightVector);
        let _e77 = (*toCameraVector);
        halfwayDir = normalize((_e76 + _e77));
        let _e80 = (*normal);
        let _e81 = halfwayDir;
        specularFactor = max(dot(_e80, _e81), 0f);
        let _e84 = (*shineDamper);
        shinePower = (_e84 * 2.4f);
    } else {
        let _e86 = reflectedLightDirection;
        let _e87 = (*toCameraVector);
        specularFactor = max(dot(_e86, _e87), 0f);
        let _e90 = (*shineDamper);
        shinePower = _e90;
    }
    let _e91 = specularFactor;
    let _e92 = shinePower;
    dampedFactor = pow(_e91, _e92);
    let _e94 = dampedFactor;
    let _e95 = (*reflectivity);
    let _e97 = (*lightColor);
    let _e99 = (*attenuationFactor);
    return ((_e97 * (_e94 * _e95)) / vec3(_e99));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e64 = (*normal_1);
    let _e65 = (*toLightVector_1);
    nDotL = dot(_e64, _e65);
    let _e67 = nDotL;
    brightness = max(_e67, 0f);
    let _e69 = brightness;
    let _e70 = (*lightColor_1);
    let _e72 = (*attenuationFactor_1);
    return ((_e70 * _e69) / vec3(_e72));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e62 = (*toLightVector_2);
    toLightDistance = length(_e62);
    let _e65 = (*attenuation)[0u];
    let _e67 = (*attenuation)[1u];
    let _e68 = toLightDistance;
    let _e72 = (*attenuation)[2u];
    let _e73 = toLightDistance;
    let _e75 = toLightDistance;
    attenuationFactor_2 = ((_e65 + (_e67 * _e68)) + ((_e72 * _e73) * _e75));
    let _e78 = attenuationFactor_2;
    return _e78;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_111_: bool;

    shadow = 1f;
    let _e68 = (*shadowCoord)[2u];
    let _e69 = (_e68 >= 0f);
    phi_111_ = _e69;
    if _e69 {
        let _e71 = (*shadowCoord)[2u];
        phi_111_ = (_e71 <= 1f);
    }
    let _e74 = phi_111_;
    if _e74 {
        let _e75 = (*shadowCoord);
        let _e77 = (*shadowCoordOffset);
        let _e78 = (_e75.xy + _e77);
        let _e79 = (*cascadeIndex);
        let _e83 = vec3<f32>(_e78.x, _e78.y, f32(_e79));
        let _e89 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e83.x, _e83.y), i32(_e83.z), 0f);
        depth = _e89.x;
        let _e91 = (*useReverseDepth);
        if (_e91 != 0u) {
            let _e93 = depth;
            let _e95 = (*shadowCoord)[2u];
            let _e96 = (*depthBias);
            if (_e93 > (_e95 + _e96)) {
                shadow = 0.2f;
            }
        } else {
            let _e99 = depth;
            let _e101 = (*shadowCoord)[2u];
            let _e102 = (*depthBias);
            if (_e99 < (_e101 - _e102)) {
                shadow = 0.2f;
            }
        }
    }
    let _e105 = shadow;
    return _e105;
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

    let _e88 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e88).xy);
    let _e92 = textureDim;
    texelSize = (vec2(1f) / _e92);
    shadow_1 = 0f;
    let _e95 = gl_FragCoord_1;
    offset = (fract((_e95.xy * 0.5f)) + vec2(0.25f));
    let _e102 = offset[1u];
    if (_e102 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e105 = texelSize;
    let _e106 = offset;
    let _e109 = (*shadowCoord_1);
    param = _e109;
    param_1 = (_e105 * (_e106 + vec2<f32>(-1.5f, 0.5f)));
    let _e110 = (*cascadeIndex_1);
    param_2 = _e110;
    let _e111 = (*depthBias_1);
    param_3 = _e111;
    let _e112 = (*useReverseDepth_1);
    param_4 = _e112;
    let _e113 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e114 = texelSize;
    let _e115 = offset;
    let _e118 = (*shadowCoord_1);
    param_5 = _e118;
    param_6 = (_e114 * (_e115 + vec2<f32>(0.5f, 0.5f)));
    let _e119 = (*cascadeIndex_1);
    param_7 = _e119;
    let _e120 = (*depthBias_1);
    param_8 = _e120;
    let _e121 = (*useReverseDepth_1);
    param_9 = _e121;
    let _e122 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e124 = texelSize;
    let _e125 = offset;
    let _e128 = (*shadowCoord_1);
    param_10 = _e128;
    param_11 = (_e124 * (_e125 + vec2<f32>(-1.5f, -1.5f)));
    let _e129 = (*cascadeIndex_1);
    param_12 = _e129;
    let _e130 = (*depthBias_1);
    param_13 = _e130;
    let _e131 = (*useReverseDepth_1);
    param_14 = _e131;
    let _e132 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e134 = texelSize;
    let _e135 = offset;
    let _e138 = (*shadowCoord_1);
    param_15 = _e138;
    param_16 = (_e134 * (_e135 + vec2<f32>(0.5f, -1.5f)));
    let _e139 = (*cascadeIndex_1);
    param_17 = _e139;
    let _e140 = (*depthBias_1);
    param_18 = _e140;
    let _e141 = (*useReverseDepth_1);
    param_19 = _e141;
    let _e142 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e113 + _e122) + _e132) + _e142) * 0.25f);
    let _e145 = shadow_1;
    return _e145;
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
        let _e74 = (*shadowCoord_2);
        param_20 = _e74;
        let _e75 = (*cascadeIndex_2);
        param_21 = _e75;
        let _e76 = (*depthBias_2);
        param_22 = _e76;
        let _e77 = (*useReverseDepth_2);
        param_23 = _e77;
        let _e78 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e78;
    } else {
        let _e79 = (*shadowCoord_2);
        param_24 = _e79;
        param_25 = vec2<f32>(0f, 0f);
        let _e80 = (*cascadeIndex_2);
        param_26 = _e80;
        let _e81 = (*depthBias_2);
        param_27 = _e81;
        let _e82 = (*useReverseDepth_2);
        param_28 = _e82;
        let _e83 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e83;
    }
    let _e84 = shadow_2;
    return _e84;
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
    let _e74 = (*depthBias_3);
    bias = _e74;
    let _e76 = (*shadows).enabled;
    if (_e76 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e78 = i;
            if (_e78 < 3u) {
                let _e81 = (*viewPosition)[2u];
                let _e82 = i;
                let _e87 = (*shadows).cascades[_e82].split[0u];
                if (_e81 < _e87) {
                    let _e89 = i;
                    cascadeIndex_3 = (_e89 + 1u);
                    let _e91 = bias;
                    bias = (_e91 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e93 = i;
                i = (_e93 + bitcast<u32>(1i));
            }
        }
        let _e96 = cascadeIndex_3;
        let _e100 = (*shadows).cascades[_e96].viewProjectionMatrix;
        let _e101 = (*worldPosition);
        shadowCoord_3 = (_e100 * vec4<f32>(_e101.x, _e101.y, _e101.z, 1f));
        let _e107 = shadowCoord_3;
        let _e109 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e107 / vec4(_e109));
        let _e112 = normalizedShadowCoord;
        param_29 = _e112;
        let _e113 = cascadeIndex_3;
        param_30 = _e113;
        let _e114 = bias;
        param_31 = _e114;
        let _e116 = (*shadows).useReverseDepth;
        param_32 = _e116;
        let _e117 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e117;
    }
    let _e118 = shadow_3;
    return _e118;
}

fn sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec3<f32> {
    var n: vec3<f32>;

    let _e63 = (*uv);
    let _e64 = (*idx);
    let _e68 = vec3<f32>(_e63.x, _e63.y, f32(_e64));
    let _e69 = (*ddx);
    let _e70 = (*ddy);
    let _e76 = textureSampleGrad(normalTextures, normalSampler, vec2<f32>(_e68.x, _e68.y), i32(_e68.z), _e69, _e70);
    n = _e76.xyz;
    let _e78 = n;
    return normalize(((normalize(_e78) * 2f) - vec3(1f)));
}

fn sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx_1: ptr<function, u32>, uv_1: ptr<function, vec2<f32>>, ddx_1: ptr<function, vec2<f32>>, ddy_1: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e62 = (*uv_1);
    let _e63 = (*idx_1);
    let _e67 = vec3<f32>(_e62.x, _e62.y, f32(_e63));
    let _e68 = (*ddx_1);
    let _e69 = (*ddy_1);
    let _e75 = textureSampleGrad(colorTextures, colorSampler, vec2<f32>(_e67.x, _e67.y), i32(_e67.z), _e68, _e69);
    return _e75;
}

fn main_1() {
    var inToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;
    var unitToCameraVector: vec3<f32>;
    var heightRange: f32;
    var normalizedHeight: f32;
    var textureColor: vec4<f32>;
    var normal_2: vec3<f32>;
    var shineDamper_1: f32;
    var reflectivity_1: f32;
    var ddx_2: vec2<f32>;
    var ddy_2: vec2<f32>;
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
    var normal1_: vec3<f32>;
    var param_41: u32;
    var param_42: vec2<f32>;
    var param_43: vec2<f32>;
    var param_44: vec2<f32>;
    var normal2_: vec3<f32>;
    var param_45: u32;
    var param_46: vec2<f32>;
    var param_47: vec2<f32>;
    var param_48: vec2<f32>;
    var shineDamper1_: f32;
    var shineDamper2_: f32;
    var reflectivity1_: f32;
    var reflectivity2_: f32;
    var param_49: u32;
    var param_50: vec2<f32>;
    var param_51: vec2<f32>;
    var param_52: vec2<f32>;
    var param_53: u32;
    var param_54: vec2<f32>;
    var param_55: vec2<f32>;
    var param_56: vec2<f32>;
    var param_57: u32;
    var param_58: vec2<f32>;
    var param_59: vec2<f32>;
    var param_60: vec2<f32>;
    var param_61: u32;
    var param_62: vec2<f32>;
    var param_63: vec2<f32>;
    var param_64: vec2<f32>;
    var shadow_4: f32;
    var param_65: Shadows;
    var param_66: vec3<f32>;
    var param_67: vec3<f32>;
    var param_68: f32;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_2: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_69: vec3<f32>;
    var param_70: vec3<f32>;
    var param_71: vec3<f32>;
    var param_72: vec3<f32>;
    var param_73: vec3<f32>;
    var param_74: f32;
    var param_75: vec3<f32>;
    var param_76: vec3<f32>;
    var param_77: vec3<f32>;
    var param_78: vec3<f32>;
    var param_79: f32;
    var param_80: f32;
    var param_81: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;
    var phi_579_: bool;

    let _e137 = inToLightVectorTangentSpace0_1;
    let _e138 = inToLightVectorTangentSpace1_1;
    let _e139 = inToLightVectorTangentSpace2_1;
    let _e140 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e137, _e138, _e139, _e140);
    let _e142 = inClipDistance_1;
    if (_e142 < 0f) {
        discard;
    }
    let _e144 = inToCameraVectorTangentSpace_1;
    let _e145 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e144 - _e145));
    let _e149 = uboFS.maxHeight;
    let _e152 = uboFS.minHeight;
    heightRange = (abs(_e149) + abs(_e152));
    let _e156 = inWorldPosition_1[1u];
    let _e158 = uboFS.minHeight;
    let _e161 = heightRange;
    normalizedHeight = ((_e156 + abs(_e158)) / _e161);
    textureColor = vec4<f32>(1f, 1f, 1f, 1f);
    normal_2 = vec3<f32>(0f, 1f, 0f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e163 = inTextureCoord_1;
    let _e164 = dpdx(_e163);
    ddx_2 = _e164;
    let _e165 = inTextureCoord_1;
    let _e166 = dpdy(_e165);
    ddy_2 = _e166;
    i_1 = 0u;
    loop {
        let _e167 = i_1;
        if (_e167 < 4u) {
            let _e169 = i_1;
            if (_e169 < 3u) {
                let _e171 = normalizedHeight;
                let _e172 = i_1;
                let _e176 = uboFS.heightSteps[_e172][0u];
                let _e178 = uboFS.heightTransitionRange;
                let _e180 = (_e171 > (_e176 - _e178));
                phi_579_ = _e180;
                if _e180 {
                    let _e181 = normalizedHeight;
                    let _e182 = i_1;
                    let _e186 = uboFS.heightSteps[_e182][0u];
                    let _e188 = uboFS.heightTransitionRange;
                    phi_579_ = (_e181 < (_e186 + _e188));
                }
                let _e192 = phi_579_;
                if _e192 {
                    let _e193 = normalizedHeight;
                    let _e194 = i_1;
                    let _e198 = uboFS.heightSteps[_e194][0u];
                    let _e201 = uboFS.heightTransitionRange;
                    let _e204 = uboFS.heightTransitionRange;
                    ratio = (((_e193 - _e198) + _e201) / (2f * _e204));
                    let _e207 = i_1;
                    param_33 = _e207;
                    let _e208 = inTextureCoord_1;
                    param_34 = _e208;
                    let _e209 = ddx_2;
                    param_35 = _e209;
                    let _e210 = ddy_2;
                    param_36 = _e210;
                    let _e211 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_33), (&param_34), (&param_35), (&param_36));
                    color1_ = _e211;
                    let _e212 = i_1;
                    param_37 = (_e212 + 1u);
                    let _e214 = inTextureCoord_1;
                    param_38 = _e214;
                    let _e215 = ddx_2;
                    param_39 = _e215;
                    let _e216 = ddy_2;
                    param_40 = _e216;
                    let _e217 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_37), (&param_38), (&param_39), (&param_40));
                    color2_ = _e217;
                    let _e218 = color1_;
                    let _e219 = color2_;
                    let _e220 = ratio;
                    textureColor = mix(_e218, _e219, vec4(_e220));
                    let _e224 = uboFS.hasNormalMap;
                    if (_e224 != 0u) {
                        let _e226 = i_1;
                        param_41 = _e226;
                        let _e227 = inTextureCoord_1;
                        param_42 = _e227;
                        let _e228 = ddx_2;
                        param_43 = _e228;
                        let _e229 = ddy_2;
                        param_44 = _e229;
                        let _e230 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_41), (&param_42), (&param_43), (&param_44));
                        normal1_ = _e230;
                        let _e231 = i_1;
                        param_45 = (_e231 + 1u);
                        let _e233 = inTextureCoord_1;
                        param_46 = _e233;
                        let _e234 = ddx_2;
                        param_47 = _e234;
                        let _e235 = ddy_2;
                        param_48 = _e235;
                        let _e236 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_45), (&param_46), (&param_47), (&param_48));
                        normal2_ = _e236;
                    } else {
                        let _e237 = inNormal_1;
                        normal1_ = _e237;
                        let _e238 = inNormal_1;
                        normal2_ = _e238;
                    }
                    let _e239 = normal1_;
                    let _e240 = normal2_;
                    let _e241 = ratio;
                    normal_2 = mix(_e239, _e240, vec3(_e241));
                    let _e244 = i_1;
                    let _e248 = uboFS.material[_e244].shineDamper;
                    shineDamper1_ = _e248;
                    let _e249 = i_1;
                    let _e254 = uboFS.material[(_e249 + 1u)].shineDamper;
                    shineDamper2_ = _e254;
                    let _e255 = shineDamper1_;
                    let _e256 = shineDamper2_;
                    let _e257 = ratio;
                    shineDamper_1 = mix(_e255, _e256, _e257);
                    let _e259 = i_1;
                    let _e263 = uboFS.material[_e259].reflectivity;
                    reflectivity1_ = _e263;
                    let _e264 = i_1;
                    let _e269 = uboFS.material[(_e264 + 1u)].reflectivity;
                    reflectivity2_ = _e269;
                    let _e270 = reflectivity1_;
                    let _e271 = reflectivity2_;
                    let _e272 = ratio;
                    reflectivity_1 = mix(_e270, _e271, _e272);
                    break;
                } else {
                    let _e274 = normalizedHeight;
                    let _e275 = i_1;
                    let _e279 = uboFS.heightSteps[_e275][0u];
                    let _e281 = uboFS.heightTransitionRange;
                    if (_e274 < (_e279 - _e281)) {
                        let _e284 = i_1;
                        param_49 = _e284;
                        let _e285 = inTextureCoord_1;
                        param_50 = _e285;
                        let _e286 = ddx_2;
                        param_51 = _e286;
                        let _e287 = ddy_2;
                        param_52 = _e287;
                        let _e288 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_49), (&param_50), (&param_51), (&param_52));
                        textureColor = _e288;
                        let _e289 = i_1;
                        param_53 = _e289;
                        let _e290 = inTextureCoord_1;
                        param_54 = _e290;
                        let _e291 = ddx_2;
                        param_55 = _e291;
                        let _e292 = ddy_2;
                        param_56 = _e292;
                        let _e293 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_53), (&param_54), (&param_55), (&param_56));
                        normal_2 = _e293;
                        let _e294 = i_1;
                        let _e298 = uboFS.material[_e294].shineDamper;
                        shineDamper_1 = _e298;
                        let _e299 = i_1;
                        let _e303 = uboFS.material[_e299].reflectivity;
                        reflectivity_1 = _e303;
                        break;
                    }
                }
            } else {
                let _e304 = i_1;
                param_57 = _e304;
                let _e305 = inTextureCoord_1;
                param_58 = _e305;
                let _e306 = ddx_2;
                param_59 = _e306;
                let _e307 = ddy_2;
                param_60 = _e307;
                let _e308 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_57), (&param_58), (&param_59), (&param_60));
                textureColor = _e308;
                let _e309 = i_1;
                param_61 = _e309;
                let _e310 = inTextureCoord_1;
                param_62 = _e310;
                let _e311 = ddx_2;
                param_63 = _e311;
                let _e312 = ddy_2;
                param_64 = _e312;
                let _e313 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_61), (&param_62), (&param_63), (&param_64));
                normal_2 = _e313;
                let _e314 = i_1;
                let _e318 = uboFS.material[_e314].shineDamper;
                shineDamper_1 = _e318;
                let _e319 = i_1;
                let _e323 = uboFS.material[_e319].reflectivity;
                reflectivity_1 = _e323;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e324 = i_1;
            i_1 = (_e324 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e328 = uboFS.castedByShadows;
    if (_e328 != 0u) {
        let _e331 = uboFS.shadows;
        param_65.cascades[0i].viewProjectionMatrix = _e331.cascades[0].viewProjectionMatrix;
        param_65.cascades[0i].split = _e331.cascades[0].split;
        param_65.cascades[1i].viewProjectionMatrix = _e331.cascades[1].viewProjectionMatrix;
        param_65.cascades[1i].split = _e331.cascades[1].split;
        param_65.cascades[2i].viewProjectionMatrix = _e331.cascades[2].viewProjectionMatrix;
        param_65.cascades[2i].split = _e331.cascades[2].split;
        param_65.cascades[3i].viewProjectionMatrix = _e331.cascades[3].viewProjectionMatrix;
        param_65.cascades[3i].split = _e331.cascades[3].split;
        param_65.enabled = _e331.enabled;
        param_65.useReverseDepth = _e331.useReverseDepth;
        let _e362 = inViewPosition_1;
        param_66 = _e362;
        let _e363 = inWorldPosition_1;
        param_67 = _e363;
        param_68 = 0.02f;
        let _e364 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_65), (&param_66), (&param_67), (&param_68));
        shadow_4 = _e364;
    }
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_2 = 0u;
    loop {
        let _e365 = i_2;
        let _e368 = uboFS.lightning.realCountOfLights;
        if (_e365 < _e368) {
            let _e370 = i_2;
            let _e374 = uboFS.lightning.lights[_e370];
            light.position = _e374.position;
            light.color = _e374.color;
            light.attenuation = _e374.attenuation;
            let _e381 = i_2;
            let _e383 = inToLightVectorTangentSpace_arr[_e381];
            let _e384 = inPositionTangentSpace_1;
            toLightVector_3 = (_e383 - _e384);
            let _e386 = toLightVector_3;
            unitToLightVector = normalize(_e386);
            let _e389 = light.attenuation;
            param_69 = _e389.xyz;
            let _e391 = toLightVector_3;
            param_70 = _e391;
            let _e392 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_69), (&param_70));
            attenuationFactor_3 = _e392;
            let _e393 = normal_2;
            param_71 = _e393;
            let _e394 = unitToLightVector;
            param_72 = _e394;
            let _e396 = light.color;
            param_73 = _e396.xyz;
            let _e398 = attenuationFactor_3;
            param_74 = _e398;
            let _e399 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_71), (&param_72), (&param_73), (&param_74));
            let _e400 = totalDiffuse;
            totalDiffuse = (_e400 + _e399);
            let _e402 = normal_2;
            param_75 = _e402;
            let _e403 = unitToLightVector;
            param_76 = _e403;
            let _e404 = unitToCameraVector;
            param_77 = _e404;
            let _e406 = light.color;
            param_78 = _e406.xyz;
            let _e408 = attenuationFactor_3;
            param_79 = _e408;
            let _e409 = shineDamper_1;
            param_80 = _e409;
            let _e410 = reflectivity_1;
            param_81 = _e410;
            let _e411 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_75), (&param_76), (&param_77), (&param_78), (&param_79), (&param_80), (&param_81));
            let _e412 = totalSpecular;
            totalSpecular = (_e412 + _e411);
            continue;
        } else {
            break;
        }
        continuing {
            let _e414 = i_2;
            i_2 = (_e414 + bitcast<u32>(1i));
        }
    }
    let _e417 = totalDiffuse;
    let _e418 = shadow_4;
    let _e422 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e417 * _e418) + vec3(_e422));
    let _e425 = totalSpecular;
    let _e426 = shadow_4;
    totalSpecular = (_e425 * _e426);
    let _e428 = totalDiffuse;
    let _e433 = textureColor;
    let _e435 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e428.x, _e428.y, _e428.z, 1f) * _e433) + vec4<f32>(_e435.x, _e435.y, _e435.z, 0f));
    let _e442 = uboFS.fogColor;
    let _e443 = _e442.xyz;
    let _e448 = baseResultColor;
    let _e449 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e443.x, _e443.y, _e443.z, 1f), _e448, vec4(_e449));
    let _e453 = uboFS.selected;
    if (_e453 != 0u) {
        let _e455 = resultColor;
        let _e457 = uboFS.selectedColor;
        resultColor = mix(_e455, _e457, vec4(0.5f));
    }
    let _e460 = resultColor;
    outColor = _e460;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(7) inToLightVectorTangentSpace0_: vec3<f32>, @location(8) inToLightVectorTangentSpace1_: vec3<f32>, @location(9) inToLightVectorTangentSpace2_: vec3<f32>, @location(10) inToLightVectorTangentSpace3_: vec3<f32>, @location(11) inClipDistance: f32, @location(5) inToCameraVectorTangentSpace: vec3<f32>, @location(6) inPositionTangentSpace: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(0) inTextureCoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inViewPosition: vec3<f32>, @location(4) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVectorTangentSpace0_1 = inToLightVectorTangentSpace0_;
    inToLightVectorTangentSpace1_1 = inToLightVectorTangentSpace1_;
    inToLightVectorTangentSpace2_1 = inToLightVectorTangentSpace2_;
    inToLightVectorTangentSpace3_1 = inToLightVectorTangentSpace3_;
    inClipDistance_1 = inClipDistance;
    inToCameraVectorTangentSpace_1 = inToCameraVectorTangentSpace;
    inPositionTangentSpace_1 = inPositionTangentSpace;
    inWorldPosition_1 = inWorldPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inViewPosition_1 = inViewPosition;
    inVisibility_1 = inVisibility;
    main_1();
    let _e27 = outColor;
    return _e27;
}
