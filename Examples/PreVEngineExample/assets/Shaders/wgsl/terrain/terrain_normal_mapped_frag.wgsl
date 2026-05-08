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
var colorTexture0_: texture_2d<f32>;
@group(0) @binding(6) 
var colorSampler: sampler;
@group(0) @binding(3) 
var colorTexture1_: texture_2d<f32>;
@group(0) @binding(4) 
var colorTexture2_: texture_2d<f32>;
@group(0) @binding(5) 
var colorTexture3_: texture_2d<f32>;
@group(0) @binding(7) 
var normalTexture0_: texture_2d<f32>;
@group(0) @binding(11) 
var normalSampler: sampler;
@group(0) @binding(8) 
var normalTexture1_: texture_2d<f32>;
@group(0) @binding(9) 
var normalTexture2_: texture_2d<f32>;
@group(0) @binding(10) 
var normalTexture3_: texture_2d<f32>;
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
@group(0) @binding(12) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(13) 
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

    let _e77 = (*toLightVector);
    lightDirection = -(_e77);
    let _e79 = lightDirection;
    let _e80 = (*normal);
    reflectedLightDirection = reflect(_e79, _e80);
    if true {
        let _e82 = (*toLightVector);
        let _e83 = (*toCameraVector);
        halfwayDir = normalize((_e82 + _e83));
        let _e86 = (*normal);
        let _e87 = halfwayDir;
        specularFactor = max(dot(_e86, _e87), 0f);
        let _e90 = (*shineDamper);
        shinePower = (_e90 * 2.4f);
    } else {
        let _e92 = reflectedLightDirection;
        let _e93 = (*toCameraVector);
        specularFactor = max(dot(_e92, _e93), 0f);
        let _e96 = (*shineDamper);
        shinePower = _e96;
    }
    let _e97 = specularFactor;
    let _e98 = shinePower;
    dampedFactor = pow(_e97, _e98);
    let _e100 = dampedFactor;
    let _e101 = (*reflectivity);
    let _e103 = (*lightColor);
    let _e105 = (*attenuationFactor);
    return ((_e103 * (_e100 * _e101)) / vec3(_e105));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e70 = (*normal_1);
    let _e71 = (*toLightVector_1);
    nDotL = dot(_e70, _e71);
    let _e73 = nDotL;
    brightness = max(_e73, 0f);
    let _e75 = brightness;
    let _e76 = (*lightColor_1);
    let _e78 = (*attenuationFactor_1);
    return ((_e76 * _e75) / vec3(_e78));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e68 = (*toLightVector_2);
    toLightDistance = length(_e68);
    let _e71 = (*attenuation)[0u];
    let _e73 = (*attenuation)[1u];
    let _e74 = toLightDistance;
    let _e78 = (*attenuation)[2u];
    let _e79 = toLightDistance;
    let _e81 = toLightDistance;
    attenuationFactor_2 = ((_e71 + (_e73 * _e74)) + ((_e78 * _e79) * _e81));
    let _e84 = attenuationFactor_2;
    return _e84;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_111_: bool;

    shadow = 1f;
    let _e74 = (*shadowCoord)[2u];
    let _e75 = (_e74 >= 0f);
    phi_111_ = _e75;
    if _e75 {
        let _e77 = (*shadowCoord)[2u];
        phi_111_ = (_e77 <= 1f);
    }
    let _e80 = phi_111_;
    if _e80 {
        let _e81 = (*shadowCoord);
        let _e83 = (*shadowCoordOffset);
        let _e84 = (_e81.xy + _e83);
        let _e85 = (*cascadeIndex);
        let _e89 = vec3<f32>(_e84.x, _e84.y, f32(_e85));
        let _e95 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e89.x, _e89.y), i32(_e89.z), 0f);
        depth = _e95.x;
        let _e97 = (*useReverseDepth);
        if (_e97 != 0u) {
            let _e99 = depth;
            let _e101 = (*shadowCoord)[2u];
            let _e102 = (*depthBias);
            if (_e99 > (_e101 + _e102)) {
                shadow = 0.2f;
            }
        } else {
            let _e105 = depth;
            let _e107 = (*shadowCoord)[2u];
            let _e108 = (*depthBias);
            if (_e105 < (_e107 - _e108)) {
                shadow = 0.2f;
            }
        }
    }
    let _e111 = shadow;
    return _e111;
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

    let _e94 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e94).xy);
    let _e98 = textureDim;
    texelSize = (vec2(1f) / _e98);
    shadow_1 = 0f;
    let _e101 = gl_FragCoord_1;
    offset = (fract((_e101.xy * 0.5f)) + vec2(0.25f));
    let _e108 = offset[1u];
    if (_e108 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e111 = texelSize;
    let _e112 = offset;
    let _e115 = (*shadowCoord_1);
    param = _e115;
    param_1 = (_e111 * (_e112 + vec2<f32>(-1.5f, 0.5f)));
    let _e116 = (*cascadeIndex_1);
    param_2 = _e116;
    let _e117 = (*depthBias_1);
    param_3 = _e117;
    let _e118 = (*useReverseDepth_1);
    param_4 = _e118;
    let _e119 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e120 = texelSize;
    let _e121 = offset;
    let _e124 = (*shadowCoord_1);
    param_5 = _e124;
    param_6 = (_e120 * (_e121 + vec2<f32>(0.5f, 0.5f)));
    let _e125 = (*cascadeIndex_1);
    param_7 = _e125;
    let _e126 = (*depthBias_1);
    param_8 = _e126;
    let _e127 = (*useReverseDepth_1);
    param_9 = _e127;
    let _e128 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e130 = texelSize;
    let _e131 = offset;
    let _e134 = (*shadowCoord_1);
    param_10 = _e134;
    param_11 = (_e130 * (_e131 + vec2<f32>(-1.5f, -1.5f)));
    let _e135 = (*cascadeIndex_1);
    param_12 = _e135;
    let _e136 = (*depthBias_1);
    param_13 = _e136;
    let _e137 = (*useReverseDepth_1);
    param_14 = _e137;
    let _e138 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e140 = texelSize;
    let _e141 = offset;
    let _e144 = (*shadowCoord_1);
    param_15 = _e144;
    param_16 = (_e140 * (_e141 + vec2<f32>(0.5f, -1.5f)));
    let _e145 = (*cascadeIndex_1);
    param_17 = _e145;
    let _e146 = (*depthBias_1);
    param_18 = _e146;
    let _e147 = (*useReverseDepth_1);
    param_19 = _e147;
    let _e148 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e119 + _e128) + _e138) + _e148) * 0.25f);
    let _e151 = shadow_1;
    return _e151;
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
        let _e80 = (*shadowCoord_2);
        param_20 = _e80;
        let _e81 = (*cascadeIndex_2);
        param_21 = _e81;
        let _e82 = (*depthBias_2);
        param_22 = _e82;
        let _e83 = (*useReverseDepth_2);
        param_23 = _e83;
        let _e84 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e84;
    } else {
        let _e85 = (*shadowCoord_2);
        param_24 = _e85;
        param_25 = vec2<f32>(0f, 0f);
        let _e86 = (*cascadeIndex_2);
        param_26 = _e86;
        let _e87 = (*depthBias_2);
        param_27 = _e87;
        let _e88 = (*useReverseDepth_2);
        param_28 = _e88;
        let _e89 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e89;
    }
    let _e90 = shadow_2;
    return _e90;
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
    let _e80 = (*depthBias_3);
    bias = _e80;
    let _e82 = (*shadows).enabled;
    if (_e82 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e84 = i;
            if (_e84 < 3u) {
                let _e87 = (*viewPosition)[2u];
                let _e88 = i;
                let _e93 = (*shadows).cascades[_e88].split[0u];
                if (_e87 < _e93) {
                    let _e95 = i;
                    cascadeIndex_3 = (_e95 + 1u);
                    let _e97 = bias;
                    bias = (_e97 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e99 = i;
                i = (_e99 + bitcast<u32>(1i));
            }
        }
        let _e102 = cascadeIndex_3;
        let _e106 = (*shadows).cascades[_e102].viewProjectionMatrix;
        let _e107 = (*worldPosition);
        shadowCoord_3 = (_e106 * vec4<f32>(_e107.x, _e107.y, _e107.z, 1f));
        let _e113 = shadowCoord_3;
        let _e115 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e113 / vec4(_e115));
        let _e118 = normalizedShadowCoord;
        param_29 = _e118;
        let _e119 = cascadeIndex_3;
        param_30 = _e119;
        let _e120 = bias;
        param_31 = _e120;
        let _e122 = (*shadows).useReverseDepth;
        param_32 = _e122;
        let _e123 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e123;
    }
    let _e124 = shadow_3;
    return _e124;
}

fn sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec3<f32> {
    var n: vec3<f32>;

    let _e69 = (*idx);
    if (_e69 == 0u) {
        let _e71 = (*uv);
        let _e72 = (*ddx);
        let _e73 = (*ddy);
        let _e74 = textureSampleGrad(normalTexture0_, normalSampler, _e71, _e72, _e73);
        n = _e74.xyz;
    } else {
        let _e76 = (*idx);
        if (_e76 == 1u) {
            let _e78 = (*uv);
            let _e79 = (*ddx);
            let _e80 = (*ddy);
            let _e81 = textureSampleGrad(normalTexture1_, normalSampler, _e78, _e79, _e80);
            n = _e81.xyz;
        } else {
            let _e83 = (*idx);
            if (_e83 == 2u) {
                let _e85 = (*uv);
                let _e86 = (*ddx);
                let _e87 = (*ddy);
                let _e88 = textureSampleGrad(normalTexture2_, normalSampler, _e85, _e86, _e87);
                n = _e88.xyz;
            } else {
                let _e90 = (*uv);
                let _e91 = (*ddx);
                let _e92 = (*ddy);
                let _e93 = textureSampleGrad(normalTexture3_, normalSampler, _e90, _e91, _e92);
                n = _e93.xyz;
            }
        }
    }
    let _e95 = n;
    return normalize(((normalize(_e95) * 2f) - vec3(1f)));
}

fn sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx_1: ptr<function, u32>, uv_1: ptr<function, vec2<f32>>, ddx_1: ptr<function, vec2<f32>>, ddy_1: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e68 = (*idx_1);
    if (_e68 == 0u) {
        let _e70 = (*uv_1);
        let _e71 = (*ddx_1);
        let _e72 = (*ddy_1);
        let _e73 = textureSampleGrad(colorTexture0_, colorSampler, _e70, _e71, _e72);
        return _e73;
    } else {
        let _e74 = (*idx_1);
        if (_e74 == 1u) {
            let _e76 = (*uv_1);
            let _e77 = (*ddx_1);
            let _e78 = (*ddy_1);
            let _e79 = textureSampleGrad(colorTexture1_, colorSampler, _e76, _e77, _e78);
            return _e79;
        } else {
            let _e80 = (*idx_1);
            if (_e80 == 2u) {
                let _e82 = (*uv_1);
                let _e83 = (*ddx_1);
                let _e84 = (*ddy_1);
                let _e85 = textureSampleGrad(colorTexture2_, colorSampler, _e82, _e83, _e84);
                return _e85;
            } else {
                let _e86 = (*uv_1);
                let _e87 = (*ddx_1);
                let _e88 = (*ddy_1);
                let _e89 = textureSampleGrad(colorTexture3_, colorSampler, _e86, _e87, _e88);
                return _e89;
            }
        }
    }
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
    var phi_656_: bool;

    let _e143 = inToLightVectorTangentSpace0_1;
    let _e144 = inToLightVectorTangentSpace1_1;
    let _e145 = inToLightVectorTangentSpace2_1;
    let _e146 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e143, _e144, _e145, _e146);
    let _e148 = inClipDistance_1;
    if (_e148 < 0f) {
        discard;
    }
    let _e150 = inToCameraVectorTangentSpace_1;
    let _e151 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e150 - _e151));
    let _e155 = uboFS.maxHeight;
    let _e158 = uboFS.minHeight;
    heightRange = (abs(_e155) + abs(_e158));
    let _e162 = inWorldPosition_1[1u];
    let _e164 = uboFS.minHeight;
    let _e167 = heightRange;
    normalizedHeight = ((_e162 + abs(_e164)) / _e167);
    textureColor = vec4<f32>(1f, 1f, 1f, 1f);
    normal_2 = vec3<f32>(0f, 1f, 0f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e169 = inTextureCoord_1;
    let _e170 = dpdx(_e169);
    ddx_2 = _e170;
    let _e171 = inTextureCoord_1;
    let _e172 = dpdy(_e171);
    ddy_2 = _e172;
    i_1 = 0u;
    loop {
        let _e173 = i_1;
        if (_e173 < 4u) {
            let _e175 = i_1;
            if (_e175 < 3u) {
                let _e177 = normalizedHeight;
                let _e178 = i_1;
                let _e182 = uboFS.heightSteps[_e178][0u];
                let _e184 = uboFS.heightTransitionRange;
                let _e186 = (_e177 > (_e182 - _e184));
                phi_656_ = _e186;
                if _e186 {
                    let _e187 = normalizedHeight;
                    let _e188 = i_1;
                    let _e192 = uboFS.heightSteps[_e188][0u];
                    let _e194 = uboFS.heightTransitionRange;
                    phi_656_ = (_e187 < (_e192 + _e194));
                }
                let _e198 = phi_656_;
                if _e198 {
                    let _e199 = normalizedHeight;
                    let _e200 = i_1;
                    let _e204 = uboFS.heightSteps[_e200][0u];
                    let _e207 = uboFS.heightTransitionRange;
                    let _e210 = uboFS.heightTransitionRange;
                    ratio = (((_e199 - _e204) + _e207) / (2f * _e210));
                    let _e213 = i_1;
                    param_33 = _e213;
                    let _e214 = inTextureCoord_1;
                    param_34 = _e214;
                    let _e215 = ddx_2;
                    param_35 = _e215;
                    let _e216 = ddy_2;
                    param_36 = _e216;
                    let _e217 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_33), (&param_34), (&param_35), (&param_36));
                    color1_ = _e217;
                    let _e218 = i_1;
                    param_37 = (_e218 + 1u);
                    let _e220 = inTextureCoord_1;
                    param_38 = _e220;
                    let _e221 = ddx_2;
                    param_39 = _e221;
                    let _e222 = ddy_2;
                    param_40 = _e222;
                    let _e223 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_37), (&param_38), (&param_39), (&param_40));
                    color2_ = _e223;
                    let _e224 = color1_;
                    let _e225 = color2_;
                    let _e226 = ratio;
                    textureColor = mix(_e224, _e225, vec4(_e226));
                    let _e230 = uboFS.hasNormalMap;
                    if (_e230 != 0u) {
                        let _e232 = i_1;
                        param_41 = _e232;
                        let _e233 = inTextureCoord_1;
                        param_42 = _e233;
                        let _e234 = ddx_2;
                        param_43 = _e234;
                        let _e235 = ddy_2;
                        param_44 = _e235;
                        let _e236 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_41), (&param_42), (&param_43), (&param_44));
                        normal1_ = _e236;
                        let _e237 = i_1;
                        param_45 = (_e237 + 1u);
                        let _e239 = inTextureCoord_1;
                        param_46 = _e239;
                        let _e240 = ddx_2;
                        param_47 = _e240;
                        let _e241 = ddy_2;
                        param_48 = _e241;
                        let _e242 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_45), (&param_46), (&param_47), (&param_48));
                        normal2_ = _e242;
                    } else {
                        let _e243 = inNormal_1;
                        normal1_ = _e243;
                        let _e244 = inNormal_1;
                        normal2_ = _e244;
                    }
                    let _e245 = normal1_;
                    let _e246 = normal2_;
                    let _e247 = ratio;
                    normal_2 = mix(_e245, _e246, vec3(_e247));
                    let _e250 = i_1;
                    let _e254 = uboFS.material[_e250].shineDamper;
                    shineDamper1_ = _e254;
                    let _e255 = i_1;
                    let _e260 = uboFS.material[(_e255 + 1u)].shineDamper;
                    shineDamper2_ = _e260;
                    let _e261 = shineDamper1_;
                    let _e262 = shineDamper2_;
                    let _e263 = ratio;
                    shineDamper_1 = mix(_e261, _e262, _e263);
                    let _e265 = i_1;
                    let _e269 = uboFS.material[_e265].reflectivity;
                    reflectivity1_ = _e269;
                    let _e270 = i_1;
                    let _e275 = uboFS.material[(_e270 + 1u)].reflectivity;
                    reflectivity2_ = _e275;
                    let _e276 = reflectivity1_;
                    let _e277 = reflectivity2_;
                    let _e278 = ratio;
                    reflectivity_1 = mix(_e276, _e277, _e278);
                    break;
                } else {
                    let _e280 = normalizedHeight;
                    let _e281 = i_1;
                    let _e285 = uboFS.heightSteps[_e281][0u];
                    let _e287 = uboFS.heightTransitionRange;
                    if (_e280 < (_e285 - _e287)) {
                        let _e290 = i_1;
                        param_49 = _e290;
                        let _e291 = inTextureCoord_1;
                        param_50 = _e291;
                        let _e292 = ddx_2;
                        param_51 = _e292;
                        let _e293 = ddy_2;
                        param_52 = _e293;
                        let _e294 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_49), (&param_50), (&param_51), (&param_52));
                        textureColor = _e294;
                        let _e295 = i_1;
                        param_53 = _e295;
                        let _e296 = inTextureCoord_1;
                        param_54 = _e296;
                        let _e297 = ddx_2;
                        param_55 = _e297;
                        let _e298 = ddy_2;
                        param_56 = _e298;
                        let _e299 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_53), (&param_54), (&param_55), (&param_56));
                        normal_2 = _e299;
                        let _e300 = i_1;
                        let _e304 = uboFS.material[_e300].shineDamper;
                        shineDamper_1 = _e304;
                        let _e305 = i_1;
                        let _e309 = uboFS.material[_e305].reflectivity;
                        reflectivity_1 = _e309;
                        break;
                    }
                }
            } else {
                let _e310 = i_1;
                param_57 = _e310;
                let _e311 = inTextureCoord_1;
                param_58 = _e311;
                let _e312 = ddx_2;
                param_59 = _e312;
                let _e313 = ddy_2;
                param_60 = _e313;
                let _e314 = sampleColorTexture_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_57), (&param_58), (&param_59), (&param_60));
                textureColor = _e314;
                let _e315 = i_1;
                param_61 = _e315;
                let _e316 = inTextureCoord_1;
                param_62 = _e316;
                let _e317 = ddx_2;
                param_63 = _e317;
                let _e318 = ddy_2;
                param_64 = _e318;
                let _e319 = sampleNormalMap_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_61), (&param_62), (&param_63), (&param_64));
                normal_2 = _e319;
                let _e320 = i_1;
                let _e324 = uboFS.material[_e320].shineDamper;
                shineDamper_1 = _e324;
                let _e325 = i_1;
                let _e329 = uboFS.material[_e325].reflectivity;
                reflectivity_1 = _e329;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e330 = i_1;
            i_1 = (_e330 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e334 = uboFS.castedByShadows;
    if (_e334 != 0u) {
        let _e337 = uboFS.shadows;
        param_65.cascades[0i].viewProjectionMatrix = _e337.cascades[0].viewProjectionMatrix;
        param_65.cascades[0i].split = _e337.cascades[0].split;
        param_65.cascades[1i].viewProjectionMatrix = _e337.cascades[1].viewProjectionMatrix;
        param_65.cascades[1i].split = _e337.cascades[1].split;
        param_65.cascades[2i].viewProjectionMatrix = _e337.cascades[2].viewProjectionMatrix;
        param_65.cascades[2i].split = _e337.cascades[2].split;
        param_65.cascades[3i].viewProjectionMatrix = _e337.cascades[3].viewProjectionMatrix;
        param_65.cascades[3i].split = _e337.cascades[3].split;
        param_65.enabled = _e337.enabled;
        param_65.useReverseDepth = _e337.useReverseDepth;
        let _e368 = inViewPosition_1;
        param_66 = _e368;
        let _e369 = inWorldPosition_1;
        param_67 = _e369;
        param_68 = 0.02f;
        let _e370 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_65), (&param_66), (&param_67), (&param_68));
        shadow_4 = _e370;
    }
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_2 = 0u;
    loop {
        let _e371 = i_2;
        let _e374 = uboFS.lightning.realCountOfLights;
        if (_e371 < _e374) {
            let _e376 = i_2;
            let _e380 = uboFS.lightning.lights[_e376];
            light.position = _e380.position;
            light.color = _e380.color;
            light.attenuation = _e380.attenuation;
            let _e387 = i_2;
            let _e389 = inToLightVectorTangentSpace_arr[_e387];
            let _e390 = inPositionTangentSpace_1;
            toLightVector_3 = (_e389 - _e390);
            let _e392 = toLightVector_3;
            unitToLightVector = normalize(_e392);
            let _e395 = light.attenuation;
            param_69 = _e395.xyz;
            let _e397 = toLightVector_3;
            param_70 = _e397;
            let _e398 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_69), (&param_70));
            attenuationFactor_3 = _e398;
            let _e399 = normal_2;
            param_71 = _e399;
            let _e400 = unitToLightVector;
            param_72 = _e400;
            let _e402 = light.color;
            param_73 = _e402.xyz;
            let _e404 = attenuationFactor_3;
            param_74 = _e404;
            let _e405 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_71), (&param_72), (&param_73), (&param_74));
            let _e406 = totalDiffuse;
            totalDiffuse = (_e406 + _e405);
            let _e408 = normal_2;
            param_75 = _e408;
            let _e409 = unitToLightVector;
            param_76 = _e409;
            let _e410 = unitToCameraVector;
            param_77 = _e410;
            let _e412 = light.color;
            param_78 = _e412.xyz;
            let _e414 = attenuationFactor_3;
            param_79 = _e414;
            let _e415 = shineDamper_1;
            param_80 = _e415;
            let _e416 = reflectivity_1;
            param_81 = _e416;
            let _e417 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_75), (&param_76), (&param_77), (&param_78), (&param_79), (&param_80), (&param_81));
            let _e418 = totalSpecular;
            totalSpecular = (_e418 + _e417);
            continue;
        } else {
            break;
        }
        continuing {
            let _e420 = i_2;
            i_2 = (_e420 + bitcast<u32>(1i));
        }
    }
    let _e423 = totalDiffuse;
    let _e424 = shadow_4;
    let _e428 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e423 * _e424) + vec3(_e428));
    let _e431 = totalSpecular;
    let _e432 = shadow_4;
    totalSpecular = (_e431 * _e432);
    let _e434 = totalDiffuse;
    let _e439 = textureColor;
    let _e441 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e434.x, _e434.y, _e434.z, 1f) * _e439) + vec4<f32>(_e441.x, _e441.y, _e441.z, 0f));
    let _e448 = uboFS.fogColor;
    let _e449 = _e448.xyz;
    let _e454 = baseResultColor;
    let _e455 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e449.x, _e449.y, _e449.z, 1f), _e454, vec4(_e455));
    let _e459 = uboFS.selected;
    if (_e459 != 0u) {
        let _e461 = resultColor;
        let _e463 = uboFS.selectedColor;
        resultColor = mix(_e461, _e463, vec4(0.5f));
    }
    let _e466 = resultColor;
    outColor = _e466;
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
