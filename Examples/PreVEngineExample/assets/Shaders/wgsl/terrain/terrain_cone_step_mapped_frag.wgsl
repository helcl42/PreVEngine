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
    heightScale: array<vec4<f32>, 4>,
    heightTransitionRange: f32,
    numLayers: u32,
    hasNormalMap: u32,
    hasConeMap: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
@group(0) @binding(12) 
var heightTexture0_: texture_2d<f32>;
@group(0) @binding(16) 
var heightSampler: sampler;
@group(0) @binding(13) 
var heightTexture1_: texture_2d<f32>;
@group(0) @binding(14) 
var heightTexture2_: texture_2d<f32>;
@group(0) @binding(15) 
var heightTexture3_: texture_2d<f32>;
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
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inPositionTangentSpace_1: vec3<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
@group(0) @binding(17) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(18) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inToCameraVectorTangentSpace_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e86 = (*toLightVector);
    lightDirection = -(_e86);
    let _e88 = lightDirection;
    let _e89 = (*normal);
    reflectedLightDirection = reflect(_e88, _e89);
    if true {
        let _e91 = (*toLightVector);
        let _e92 = (*toCameraVector);
        halfwayDir = normalize((_e91 + _e92));
        let _e95 = (*normal);
        let _e96 = halfwayDir;
        specularFactor = max(dot(_e95, _e96), 0f);
        let _e99 = (*shineDamper);
        shinePower = (_e99 * 2.4f);
    } else {
        let _e101 = reflectedLightDirection;
        let _e102 = (*toCameraVector);
        specularFactor = max(dot(_e101, _e102), 0f);
        let _e105 = (*shineDamper);
        shinePower = _e105;
    }
    let _e106 = specularFactor;
    let _e107 = shinePower;
    dampedFactor = pow(_e106, _e107);
    let _e109 = dampedFactor;
    let _e110 = (*reflectivity);
    let _e112 = (*lightColor);
    let _e114 = (*attenuationFactor);
    return ((_e112 * (_e109 * _e110)) / vec3(_e114));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e79 = (*normal_1);
    let _e80 = (*toLightVector_1);
    nDotL = dot(_e79, _e80);
    let _e82 = nDotL;
    brightness = max(_e82, 0f);
    let _e84 = brightness;
    let _e85 = (*lightColor_1);
    let _e87 = (*attenuationFactor_1);
    return ((_e85 * _e84) / vec3(_e87));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e77 = (*toLightVector_2);
    toLightDistance = length(_e77);
    let _e80 = (*attenuation)[0u];
    let _e82 = (*attenuation)[1u];
    let _e83 = toLightDistance;
    let _e87 = (*attenuation)[2u];
    let _e88 = toLightDistance;
    let _e90 = toLightDistance;
    attenuationFactor_2 = ((_e80 + (_e82 * _e83)) + ((_e87 * _e88) * _e90));
    let _e93 = attenuationFactor_2;
    return _e93;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_130_: bool;

    shadow = 1f;
    let _e83 = (*shadowCoord)[2u];
    let _e84 = (_e83 >= 0f);
    phi_130_ = _e84;
    if _e84 {
        let _e86 = (*shadowCoord)[2u];
        phi_130_ = (_e86 <= 1f);
    }
    let _e89 = phi_130_;
    if _e89 {
        let _e90 = (*shadowCoord);
        let _e92 = (*shadowCoordOffset);
        let _e93 = (_e90.xy + _e92);
        let _e94 = (*cascadeIndex);
        let _e98 = vec3<f32>(_e93.x, _e93.y, f32(_e94));
        let _e104 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e98.x, _e98.y), i32(_e98.z), 0f);
        depth = _e104.x;
        let _e106 = (*useReverseDepth);
        if (_e106 != 0u) {
            let _e108 = depth;
            let _e110 = (*shadowCoord)[2u];
            let _e111 = (*depthBias);
            if (_e108 > (_e110 + _e111)) {
                shadow = 0.2f;
            }
        } else {
            let _e114 = depth;
            let _e116 = (*shadowCoord)[2u];
            let _e117 = (*depthBias);
            if (_e114 < (_e116 - _e117)) {
                shadow = 0.2f;
            }
        }
    }
    let _e120 = shadow;
    return _e120;
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

    let _e103 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e103).xy);
    let _e107 = textureDim;
    texelSize = (vec2(1f) / _e107);
    shadow_1 = 0f;
    let _e110 = gl_FragCoord_1;
    offset = (fract((_e110.xy * 0.5f)) + vec2(0.25f));
    let _e117 = offset[1u];
    if (_e117 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e120 = texelSize;
    let _e121 = offset;
    let _e124 = (*shadowCoord_1);
    param = _e124;
    param_1 = (_e120 * (_e121 + vec2<f32>(-1.5f, 0.5f)));
    let _e125 = (*cascadeIndex_1);
    param_2 = _e125;
    let _e126 = (*depthBias_1);
    param_3 = _e126;
    let _e127 = (*useReverseDepth_1);
    param_4 = _e127;
    let _e128 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e129 = texelSize;
    let _e130 = offset;
    let _e133 = (*shadowCoord_1);
    param_5 = _e133;
    param_6 = (_e129 * (_e130 + vec2<f32>(0.5f, 0.5f)));
    let _e134 = (*cascadeIndex_1);
    param_7 = _e134;
    let _e135 = (*depthBias_1);
    param_8 = _e135;
    let _e136 = (*useReverseDepth_1);
    param_9 = _e136;
    let _e137 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e139 = texelSize;
    let _e140 = offset;
    let _e143 = (*shadowCoord_1);
    param_10 = _e143;
    param_11 = (_e139 * (_e140 + vec2<f32>(-1.5f, -1.5f)));
    let _e144 = (*cascadeIndex_1);
    param_12 = _e144;
    let _e145 = (*depthBias_1);
    param_13 = _e145;
    let _e146 = (*useReverseDepth_1);
    param_14 = _e146;
    let _e147 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e149 = texelSize;
    let _e150 = offset;
    let _e153 = (*shadowCoord_1);
    param_15 = _e153;
    param_16 = (_e149 * (_e150 + vec2<f32>(0.5f, -1.5f)));
    let _e154 = (*cascadeIndex_1);
    param_17 = _e154;
    let _e155 = (*depthBias_1);
    param_18 = _e155;
    let _e156 = (*useReverseDepth_1);
    param_19 = _e156;
    let _e157 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e128 + _e137) + _e147) + _e157) * 0.25f);
    let _e160 = shadow_1;
    return _e160;
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
        let _e89 = (*shadowCoord_2);
        param_20 = _e89;
        let _e90 = (*cascadeIndex_2);
        param_21 = _e90;
        let _e91 = (*depthBias_2);
        param_22 = _e91;
        let _e92 = (*useReverseDepth_2);
        param_23 = _e92;
        let _e93 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e93;
    } else {
        let _e94 = (*shadowCoord_2);
        param_24 = _e94;
        param_25 = vec2<f32>(0f, 0f);
        let _e95 = (*cascadeIndex_2);
        param_26 = _e95;
        let _e96 = (*depthBias_2);
        param_27 = _e96;
        let _e97 = (*useReverseDepth_2);
        param_28 = _e97;
        let _e98 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e98;
    }
    let _e99 = shadow_2;
    return _e99;
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
    let _e89 = (*depthBias_3);
    bias = _e89;
    let _e91 = (*shadows).enabled;
    if (_e91 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e93 = i;
            if (_e93 < 3u) {
                let _e96 = (*viewPosition)[2u];
                let _e97 = i;
                let _e102 = (*shadows).cascades[_e97].split[0u];
                if (_e96 < _e102) {
                    let _e104 = i;
                    cascadeIndex_3 = (_e104 + 1u);
                    let _e106 = bias;
                    bias = (_e106 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e108 = i;
                i = (_e108 + bitcast<u32>(1i));
            }
        }
        let _e111 = cascadeIndex_3;
        let _e115 = (*shadows).cascades[_e111].viewProjectionMatrix;
        let _e116 = (*worldPosition);
        shadowCoord_3 = (_e115 * vec4<f32>(_e116.x, _e116.y, _e116.z, 1f));
        let _e122 = shadowCoord_3;
        let _e124 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e122 / vec4(_e124));
        let _e127 = normalizedShadowCoord;
        param_29 = _e127;
        let _e128 = cascadeIndex_3;
        param_30 = _e128;
        let _e129 = bias;
        param_31 = _e129;
        let _e131 = (*shadows).useReverseDepth;
        param_32 = _e131;
        let _e132 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e132;
    }
    let _e133 = shadow_3;
    return _e133;
}

fn sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx: ptr<function, u32>, uv: ptr<function, vec2<f32>>, ddx: ptr<function, vec2<f32>>, ddy: ptr<function, vec2<f32>>) -> vec4<f32> {
    let _e77 = (*idx);
    if (_e77 == 0u) {
        let _e79 = (*uv);
        let _e80 = (*ddx);
        let _e81 = (*ddy);
        let _e82 = textureSampleGrad(colorTexture0_, colorSampler, _e79, _e80, _e81);
        return _e82;
    } else {
        let _e83 = (*idx);
        if (_e83 == 1u) {
            let _e85 = (*uv);
            let _e86 = (*ddx);
            let _e87 = (*ddy);
            let _e88 = textureSampleGrad(colorTexture1_, colorSampler, _e85, _e86, _e87);
            return _e88;
        } else {
            let _e89 = (*idx);
            if (_e89 == 2u) {
                let _e91 = (*uv);
                let _e92 = (*ddx);
                let _e93 = (*ddy);
                let _e94 = textureSampleGrad(colorTexture2_, colorSampler, _e91, _e92, _e93);
                return _e94;
            } else {
                let _e95 = (*uv);
                let _e96 = (*ddx);
                let _e97 = (*ddy);
                let _e98 = textureSampleGrad(colorTexture3_, colorSampler, _e95, _e96, _e97);
                return _e98;
            }
        }
    }
}

fn sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b(idx_1: ptr<function, u32>, uv_1: ptr<function, vec2<f32>>, ddx_1: ptr<function, vec2<f32>>, ddy_1: ptr<function, vec2<f32>>) -> vec3<f32> {
    var n: vec3<f32>;

    let _e78 = (*idx_1);
    if (_e78 == 0u) {
        let _e80 = (*uv_1);
        let _e81 = (*ddx_1);
        let _e82 = (*ddy_1);
        let _e83 = textureSampleGrad(normalTexture0_, normalSampler, _e80, _e81, _e82);
        n = _e83.xyz;
    } else {
        let _e85 = (*idx_1);
        if (_e85 == 1u) {
            let _e87 = (*uv_1);
            let _e88 = (*ddx_1);
            let _e89 = (*ddy_1);
            let _e90 = textureSampleGrad(normalTexture1_, normalSampler, _e87, _e88, _e89);
            n = _e90.xyz;
        } else {
            let _e92 = (*idx_1);
            if (_e92 == 2u) {
                let _e94 = (*uv_1);
                let _e95 = (*ddx_1);
                let _e96 = (*ddy_1);
                let _e97 = textureSampleGrad(normalTexture2_, normalSampler, _e94, _e95, _e96);
                n = _e97.xyz;
            } else {
                let _e99 = (*uv_1);
                let _e100 = (*ddx_1);
                let _e101 = (*ddy_1);
                let _e102 = textureSampleGrad(normalTexture3_, normalSampler, _e99, _e100, _e101);
                n = _e102.xyz;
            }
        }
    }
    let _e104 = n;
    return normalize(((normalize(_e104) * 2f) - vec3(1f)));
}

fn GetInverseHeight_u0028_f1_u003b(height: ptr<function, f32>) -> f32 {
    let _e74 = (*height);
    return (1f - _e74);
}

fn GetRayDirection_u0028_vf3_u003b_f1_u003b(viewDirection: ptr<function, vec3<f32>>, heightScale: ptr<function, f32>) -> vec3<f32> {
    var v: vec3<f32>;
    var db: f32;

    let _e77 = (*viewDirection);
    v = normalize(_e77);
    let _e80 = v[2u];
    v[2u] = abs(_e80);
    if true {
        let _e84 = v[2u];
        db = (1f - _e84);
        let _e86 = db;
        let _e87 = db;
        db = (_e87 * _e86);
        let _e89 = db;
        let _e90 = db;
        db = (_e90 * _e89);
        let _e92 = db;
        let _e93 = db;
        db = (1f - (_e92 * _e93));
        let _e96 = db;
        let _e97 = v;
        let _e99 = (_e97.xy * _e96);
        v[0u] = _e99.x;
        v[1u] = _e99.y;
    }
    let _e104 = (*heightScale);
    let _e105 = v;
    let _e107 = (_e105.xy * _e104);
    v[0u] = _e107.x;
    v[1u] = _e107.y;
    let _e112 = v;
    return _e112;
}

fn sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b(idx_2: ptr<function, u32>, heightScale_1: ptr<function, f32>, numLayers: ptr<function, u32>, uv_2: ptr<function, vec2<f32>>, ddx_2: ptr<function, vec2<f32>>, ddy_2: ptr<function, vec2<f32>>, texDir3D: ptr<function, vec3<f32>>) -> vec2<f32> {
    var rayPos: vec3<f32>;
    var rayDir: vec3<f32>;
    var param_33: vec3<f32>;
    var param_34: f32;
    var rayRatio: f32;
    var pos: vec3<f32>;
    var i_1: u32;
    var heightAndCone: vec2<f32>;
    var coneRatio: f32;
    var height_1: f32;
    var param_35: f32;
    var d: f32;
    var bsRange: vec3<f32>;
    var bsPosition: vec3<f32>;
    var i_2: u32;
    var heightAndCone_1: vec2<f32>;
    var param_36: f32;

    let _e97 = (*uv_2);
    rayPos = vec3<f32>(_e97.x, _e97.y, 0f);
    let _e101 = (*texDir3D);
    param_33 = _e101;
    let _e102 = (*heightScale_1);
    param_34 = _e102;
    let _e103 = GetRayDirection_u0028_vf3_u003b_f1_u003b((&param_33), (&param_34));
    rayDir = _e103;
    let _e105 = rayDir[2u];
    let _e106 = rayDir;
    rayDir = (_e106 / vec3(_e105));
    let _e109 = rayDir;
    rayRatio = length(_e109.xy);
    let _e112 = rayPos;
    pos = _e112;
    i_1 = 0u;
    loop {
        let _e113 = i_1;
        let _e114 = (*numLayers);
        if (_e113 < _e114) {
            let _e116 = (*idx_2);
            if (_e116 == 0u) {
                let _e118 = pos;
                let _e120 = (*ddx_2);
                let _e121 = (*ddy_2);
                let _e122 = textureSampleGrad(heightTexture0_, heightSampler, _e118.xy, _e120, _e121);
                heightAndCone = clamp(_e122.xy, vec2(0f), vec2(1f));
            } else {
                let _e127 = (*idx_2);
                if (_e127 == 1u) {
                    let _e129 = pos;
                    let _e131 = (*ddx_2);
                    let _e132 = (*ddy_2);
                    let _e133 = textureSampleGrad(heightTexture1_, heightSampler, _e129.xy, _e131, _e132);
                    heightAndCone = clamp(_e133.xy, vec2(0f), vec2(1f));
                } else {
                    let _e138 = (*idx_2);
                    if (_e138 == 2u) {
                        let _e140 = pos;
                        let _e142 = (*ddx_2);
                        let _e143 = (*ddy_2);
                        let _e144 = textureSampleGrad(heightTexture2_, heightSampler, _e140.xy, _e142, _e143);
                        heightAndCone = clamp(_e144.xy, vec2(0f), vec2(1f));
                    } else {
                        let _e149 = pos;
                        let _e151 = (*ddx_2);
                        let _e152 = (*ddy_2);
                        let _e153 = textureSampleGrad(heightTexture3_, heightSampler, _e149.xy, _e151, _e152);
                        heightAndCone = clamp(_e153.xy, vec2(0f), vec2(1f));
                    }
                }
            }
            let _e159 = heightAndCone[1u];
            let _e161 = heightAndCone[1u];
            coneRatio = (_e159 * _e161);
            let _e164 = heightAndCone[0u];
            param_35 = _e164;
            let _e165 = GetInverseHeight_u0028_f1_u003b((&param_35));
            let _e167 = pos[2u];
            height_1 = (_e165 - _e167);
            let _e169 = coneRatio;
            let _e170 = height_1;
            let _e172 = rayRatio;
            let _e173 = coneRatio;
            d = ((_e169 * _e170) / (_e172 + _e173));
            let _e176 = rayDir;
            let _e177 = d;
            let _e179 = pos;
            pos = (_e179 + (_e176 * _e177));
            continue;
        } else {
            break;
        }
        continuing {
            let _e181 = i_1;
            i_1 = (_e181 + bitcast<u32>(1i));
        }
    }
    let _e184 = rayDir;
    let _e187 = pos[2u];
    bsRange = ((_e184 * 0.5f) * _e187);
    let _e189 = rayPos;
    let _e190 = bsRange;
    bsPosition = (_e189 + _e190);
    i_2 = 0u;
    loop {
        let _e192 = i_2;
        if (_e192 < 6u) {
            let _e194 = (*idx_2);
            if (_e194 == 0u) {
                let _e196 = pos;
                let _e198 = (*ddx_2);
                let _e199 = (*ddy_2);
                let _e200 = textureSampleGrad(heightTexture0_, heightSampler, _e196.xy, _e198, _e199);
                heightAndCone_1 = clamp(_e200.xy, vec2(0f), vec2(1f));
            } else {
                let _e205 = (*idx_2);
                if (_e205 == 1u) {
                    let _e207 = pos;
                    let _e209 = (*ddx_2);
                    let _e210 = (*ddy_2);
                    let _e211 = textureSampleGrad(heightTexture1_, heightSampler, _e207.xy, _e209, _e210);
                    heightAndCone_1 = clamp(_e211.xy, vec2(0f), vec2(1f));
                } else {
                    let _e216 = (*idx_2);
                    if (_e216 == 2u) {
                        let _e218 = pos;
                        let _e220 = (*ddx_2);
                        let _e221 = (*ddy_2);
                        let _e222 = textureSampleGrad(heightTexture2_, heightSampler, _e218.xy, _e220, _e221);
                        heightAndCone_1 = clamp(_e222.xy, vec2(0f), vec2(1f));
                    } else {
                        let _e227 = pos;
                        let _e229 = (*ddx_2);
                        let _e230 = (*ddy_2);
                        let _e231 = textureSampleGrad(heightTexture3_, heightSampler, _e227.xy, _e229, _e230);
                        heightAndCone_1 = clamp(_e231.xy, vec2(0f), vec2(1f));
                    }
                }
            }
            let _e236 = bsRange;
            bsRange = (_e236 * 0.5f);
            let _e239 = bsPosition[2u];
            let _e241 = heightAndCone_1[0u];
            param_36 = _e241;
            let _e242 = GetInverseHeight_u0028_f1_u003b((&param_36));
            if (_e239 < _e242) {
                let _e244 = bsRange;
                let _e245 = bsPosition;
                bsPosition = (_e245 + _e244);
            } else {
                let _e247 = bsRange;
                let _e248 = bsPosition;
                bsPosition = (_e248 - _e247);
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e250 = i_2;
            i_2 = (_e250 + bitcast<u32>(1i));
        }
    }
    let _e253 = bsPosition;
    return _e253.xy;
}

fn main_1() {
    var inToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;
    var heightRange: f32;
    var normalizedHeight: f32;
    var rayDirection: vec3<f32>;
    var textureColor: vec4<f32>;
    var normal_2: vec3<f32>;
    var shineDamper_1: f32;
    var reflectivity_1: f32;
    var ddx_3: vec2<f32>;
    var ddy_3: vec2<f32>;
    var i_3: u32;
    var ratio: f32;
    var uv1_: vec2<f32>;
    var param_37: u32;
    var param_38: f32;
    var param_39: u32;
    var param_40: vec2<f32>;
    var param_41: vec2<f32>;
    var param_42: vec2<f32>;
    var param_43: vec3<f32>;
    var uv2_: vec2<f32>;
    var param_44: u32;
    var param_45: f32;
    var param_46: u32;
    var param_47: vec2<f32>;
    var param_48: vec2<f32>;
    var param_49: vec2<f32>;
    var param_50: vec3<f32>;
    var normal1_: vec3<f32>;
    var param_51: u32;
    var param_52: vec2<f32>;
    var param_53: vec2<f32>;
    var param_54: vec2<f32>;
    var normal2_: vec3<f32>;
    var param_55: u32;
    var param_56: vec2<f32>;
    var param_57: vec2<f32>;
    var param_58: vec2<f32>;
    var color1_: vec4<f32>;
    var param_59: u32;
    var param_60: vec2<f32>;
    var param_61: vec2<f32>;
    var param_62: vec2<f32>;
    var color2_: vec4<f32>;
    var param_63: u32;
    var param_64: vec2<f32>;
    var param_65: vec2<f32>;
    var param_66: vec2<f32>;
    var shineDamper1_: f32;
    var shineDamper2_: f32;
    var reflectivity1_: f32;
    var reflectivity2_: f32;
    var uv_3: vec2<f32>;
    var param_67: u32;
    var param_68: f32;
    var param_69: u32;
    var param_70: vec2<f32>;
    var param_71: vec2<f32>;
    var param_72: vec2<f32>;
    var param_73: vec3<f32>;
    var param_74: u32;
    var param_75: vec2<f32>;
    var param_76: vec2<f32>;
    var param_77: vec2<f32>;
    var param_78: u32;
    var param_79: vec2<f32>;
    var param_80: vec2<f32>;
    var param_81: vec2<f32>;
    var uv_4: vec2<f32>;
    var param_82: u32;
    var param_83: f32;
    var param_84: u32;
    var param_85: vec2<f32>;
    var param_86: vec2<f32>;
    var param_87: vec2<f32>;
    var param_88: vec3<f32>;
    var param_89: u32;
    var param_90: vec2<f32>;
    var param_91: vec2<f32>;
    var param_92: vec2<f32>;
    var param_93: u32;
    var param_94: vec2<f32>;
    var param_95: vec2<f32>;
    var param_96: vec2<f32>;
    var shadow_4: f32;
    var param_97: Shadows;
    var param_98: vec3<f32>;
    var param_99: vec3<f32>;
    var param_100: f32;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_4: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_101: vec3<f32>;
    var param_102: vec3<f32>;
    var param_103: vec3<f32>;
    var param_104: vec3<f32>;
    var param_105: vec3<f32>;
    var param_106: f32;
    var param_107: vec3<f32>;
    var param_108: vec3<f32>;
    var param_109: vec3<f32>;
    var param_110: vec3<f32>;
    var param_111: f32;
    var param_112: f32;
    var param_113: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;
    var phi_955_: bool;

    let _e185 = inToLightVectorTangentSpace0_1;
    let _e186 = inToLightVectorTangentSpace1_1;
    let _e187 = inToLightVectorTangentSpace2_1;
    let _e188 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e185, _e186, _e187, _e188);
    let _e190 = inClipDistance_1;
    if (_e190 < 0f) {
        discard;
    }
    let _e193 = uboFS.maxHeight;
    let _e196 = uboFS.minHeight;
    heightRange = (abs(_e193) + abs(_e196));
    let _e200 = inWorldPosition_1[1u];
    let _e202 = uboFS.minHeight;
    let _e205 = heightRange;
    normalizedHeight = ((_e200 + abs(_e202)) / _e205);
    let _e207 = inPositionTangentSpace_1;
    rayDirection = normalize(_e207);
    textureColor = vec4<f32>(1f, 1f, 1f, 1f);
    normal_2 = vec3<f32>(0f, 1f, 0f);
    shineDamper_1 = 1f;
    reflectivity_1 = 1f;
    let _e209 = inTextureCoord_1;
    let _e210 = dpdx(_e209);
    ddx_3 = _e210;
    let _e211 = inTextureCoord_1;
    let _e212 = dpdy(_e211);
    ddy_3 = _e212;
    i_3 = 0u;
    loop {
        let _e213 = i_3;
        if (_e213 < 4u) {
            let _e215 = i_3;
            if (_e215 < 3u) {
                let _e217 = normalizedHeight;
                let _e218 = i_3;
                let _e222 = uboFS.heightSteps[_e218][0u];
                let _e224 = uboFS.heightTransitionRange;
                let _e226 = (_e217 > (_e222 - _e224));
                phi_955_ = _e226;
                if _e226 {
                    let _e227 = normalizedHeight;
                    let _e228 = i_3;
                    let _e232 = uboFS.heightSteps[_e228][0u];
                    let _e234 = uboFS.heightTransitionRange;
                    phi_955_ = (_e227 < (_e232 + _e234));
                }
                let _e238 = phi_955_;
                if _e238 {
                    let _e239 = normalizedHeight;
                    let _e240 = i_3;
                    let _e244 = uboFS.heightSteps[_e240][0u];
                    let _e247 = uboFS.heightTransitionRange;
                    let _e250 = uboFS.heightTransitionRange;
                    ratio = (((_e239 - _e244) + _e247) / (2f * _e250));
                    let _e254 = uboFS.hasConeMap;
                    if (_e254 != 0u) {
                        let _e256 = i_3;
                        let _e257 = i_3;
                        param_37 = _e257;
                        let _e261 = uboFS.heightScale[_e256][0u];
                        param_38 = _e261;
                        let _e263 = uboFS.numLayers;
                        param_39 = _e263;
                        let _e264 = inTextureCoord_1;
                        param_40 = _e264;
                        let _e265 = ddx_3;
                        param_41 = _e265;
                        let _e266 = ddy_3;
                        param_42 = _e266;
                        let _e267 = rayDirection;
                        param_43 = _e267;
                        let _e268 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_37), (&param_38), (&param_39), (&param_40), (&param_41), (&param_42), (&param_43));
                        uv1_ = _e268;
                        let _e269 = i_3;
                        let _e271 = i_3;
                        param_44 = (_e269 + 1u);
                        let _e276 = uboFS.heightScale[(_e271 + 1u)][0u];
                        param_45 = _e276;
                        let _e278 = uboFS.numLayers;
                        param_46 = _e278;
                        let _e279 = inTextureCoord_1;
                        param_47 = _e279;
                        let _e280 = ddx_3;
                        param_48 = _e280;
                        let _e281 = ddy_3;
                        param_49 = _e281;
                        let _e282 = rayDirection;
                        param_50 = _e282;
                        let _e283 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_44), (&param_45), (&param_46), (&param_47), (&param_48), (&param_49), (&param_50));
                        uv2_ = _e283;
                    } else {
                        let _e284 = inTextureCoord_1;
                        uv1_ = _e284;
                        let _e285 = inTextureCoord_1;
                        uv2_ = _e285;
                    }
                    let _e287 = uboFS.hasNormalMap;
                    if (_e287 != 0u) {
                        let _e289 = i_3;
                        param_51 = _e289;
                        let _e290 = uv1_;
                        param_52 = _e290;
                        let _e291 = ddx_3;
                        param_53 = _e291;
                        let _e292 = ddy_3;
                        param_54 = _e292;
                        let _e293 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_51), (&param_52), (&param_53), (&param_54));
                        normal1_ = _e293;
                        let _e294 = i_3;
                        param_55 = (_e294 + 1u);
                        let _e296 = uv2_;
                        param_56 = _e296;
                        let _e297 = ddx_3;
                        param_57 = _e297;
                        let _e298 = ddy_3;
                        param_58 = _e298;
                        let _e299 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_55), (&param_56), (&param_57), (&param_58));
                        normal2_ = _e299;
                    } else {
                        let _e300 = inNormal_1;
                        normal1_ = _e300;
                        let _e301 = inNormal_1;
                        normal2_ = _e301;
                    }
                    let _e302 = normal1_;
                    let _e303 = normal2_;
                    let _e304 = ratio;
                    normal_2 = mix(_e302, _e303, vec3(_e304));
                    let _e307 = i_3;
                    param_59 = _e307;
                    let _e308 = uv1_;
                    param_60 = _e308;
                    let _e309 = ddx_3;
                    param_61 = _e309;
                    let _e310 = ddy_3;
                    param_62 = _e310;
                    let _e311 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_59), (&param_60), (&param_61), (&param_62));
                    color1_ = _e311;
                    let _e312 = i_3;
                    param_63 = (_e312 + 1u);
                    let _e314 = uv2_;
                    param_64 = _e314;
                    let _e315 = ddx_3;
                    param_65 = _e315;
                    let _e316 = ddy_3;
                    param_66 = _e316;
                    let _e317 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_63), (&param_64), (&param_65), (&param_66));
                    color2_ = _e317;
                    let _e318 = color1_;
                    let _e319 = color2_;
                    let _e320 = ratio;
                    textureColor = mix(_e318, _e319, vec4(_e320));
                    let _e323 = i_3;
                    let _e327 = uboFS.material[_e323].shineDamper;
                    shineDamper1_ = _e327;
                    let _e328 = i_3;
                    let _e333 = uboFS.material[(_e328 + 1u)].shineDamper;
                    shineDamper2_ = _e333;
                    let _e334 = shineDamper1_;
                    let _e335 = shineDamper2_;
                    let _e336 = ratio;
                    shineDamper_1 = mix(_e334, _e335, _e336);
                    let _e338 = i_3;
                    let _e342 = uboFS.material[_e338].reflectivity;
                    reflectivity1_ = _e342;
                    let _e343 = i_3;
                    let _e348 = uboFS.material[(_e343 + 1u)].reflectivity;
                    reflectivity2_ = _e348;
                    let _e349 = reflectivity1_;
                    let _e350 = reflectivity2_;
                    let _e351 = ratio;
                    reflectivity_1 = mix(_e349, _e350, _e351);
                    break;
                } else {
                    let _e353 = normalizedHeight;
                    let _e354 = i_3;
                    let _e358 = uboFS.heightSteps[_e354][0u];
                    let _e360 = uboFS.heightTransitionRange;
                    if (_e353 < (_e358 - _e360)) {
                        let _e363 = i_3;
                        let _e364 = i_3;
                        param_67 = _e364;
                        let _e368 = uboFS.heightScale[_e363][0u];
                        param_68 = _e368;
                        let _e370 = uboFS.numLayers;
                        param_69 = _e370;
                        let _e371 = inTextureCoord_1;
                        param_70 = _e371;
                        let _e372 = ddx_3;
                        param_71 = _e372;
                        let _e373 = ddy_3;
                        param_72 = _e373;
                        let _e374 = rayDirection;
                        param_73 = _e374;
                        let _e375 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_67), (&param_68), (&param_69), (&param_70), (&param_71), (&param_72), (&param_73));
                        uv_3 = _e375;
                        let _e376 = i_3;
                        param_74 = _e376;
                        let _e377 = uv_3;
                        param_75 = _e377;
                        let _e378 = ddx_3;
                        param_76 = _e378;
                        let _e379 = ddy_3;
                        param_77 = _e379;
                        let _e380 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_74), (&param_75), (&param_76), (&param_77));
                        normal_2 = _e380;
                        let _e381 = i_3;
                        param_78 = _e381;
                        let _e382 = uv_3;
                        param_79 = _e382;
                        let _e383 = ddx_3;
                        param_80 = _e383;
                        let _e384 = ddy_3;
                        param_81 = _e384;
                        let _e385 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_78), (&param_79), (&param_80), (&param_81));
                        textureColor = _e385;
                        let _e386 = i_3;
                        let _e390 = uboFS.material[_e386].shineDamper;
                        shineDamper_1 = _e390;
                        let _e391 = i_3;
                        let _e395 = uboFS.material[_e391].reflectivity;
                        reflectivity_1 = _e395;
                        break;
                    }
                }
            } else {
                let _e396 = i_3;
                let _e397 = i_3;
                param_82 = _e397;
                let _e401 = uboFS.heightScale[_e396][0u];
                param_83 = _e401;
                let _e403 = uboFS.numLayers;
                param_84 = _e403;
                let _e404 = inTextureCoord_1;
                param_85 = _e404;
                let _e405 = ddx_3;
                param_86 = _e405;
                let _e406 = ddy_3;
                param_87 = _e406;
                let _e407 = rayDirection;
                param_88 = _e407;
                let _e408 = sampleRelaxedConeStepMapping_u0028_u1_u003b_f1_u003b_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b_vf3_u003b((&param_82), (&param_83), (&param_84), (&param_85), (&param_86), (&param_87), (&param_88));
                uv_4 = _e408;
                let _e409 = i_3;
                param_89 = _e409;
                let _e410 = uv_4;
                param_90 = _e410;
                let _e411 = ddx_3;
                param_91 = _e411;
                let _e412 = ddy_3;
                param_92 = _e412;
                let _e413 = sampleNormalMapGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_89), (&param_90), (&param_91), (&param_92));
                normal_2 = _e413;
                let _e414 = i_3;
                param_93 = _e414;
                let _e415 = uv_4;
                param_94 = _e415;
                let _e416 = ddx_3;
                param_95 = _e416;
                let _e417 = ddy_3;
                param_96 = _e417;
                let _e418 = sampleColorTextureGrad_u0028_u1_u003b_vf2_u003b_vf2_u003b_vf2_u003b((&param_93), (&param_94), (&param_95), (&param_96));
                textureColor = _e418;
                let _e419 = i_3;
                let _e423 = uboFS.material[_e419].shineDamper;
                shineDamper_1 = _e423;
                let _e424 = i_3;
                let _e428 = uboFS.material[_e424].reflectivity;
                reflectivity_1 = _e428;
            }
            continue;
        } else {
            break;
        }
        continuing {
            let _e429 = i_3;
            i_3 = (_e429 + bitcast<u32>(1i));
        }
    }
    shadow_4 = 1f;
    let _e433 = uboFS.castedByShadows;
    if (_e433 != 0u) {
        let _e436 = uboFS.shadows;
        param_97.cascades[0i].viewProjectionMatrix = _e436.cascades[0].viewProjectionMatrix;
        param_97.cascades[0i].split = _e436.cascades[0].split;
        param_97.cascades[1i].viewProjectionMatrix = _e436.cascades[1].viewProjectionMatrix;
        param_97.cascades[1i].split = _e436.cascades[1].split;
        param_97.cascades[2i].viewProjectionMatrix = _e436.cascades[2].viewProjectionMatrix;
        param_97.cascades[2i].split = _e436.cascades[2].split;
        param_97.cascades[3i].viewProjectionMatrix = _e436.cascades[3].viewProjectionMatrix;
        param_97.cascades[3i].split = _e436.cascades[3].split;
        param_97.enabled = _e436.enabled;
        param_97.useReverseDepth = _e436.useReverseDepth;
        let _e467 = inViewPosition_1;
        param_98 = _e467;
        let _e468 = inWorldPosition_1;
        param_99 = _e468;
        param_100 = 0.02f;
        let _e469 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_97), (&param_98), (&param_99), (&param_100));
        shadow_4 = _e469;
    }
    let _e470 = inToCameraVectorTangentSpace_1;
    let _e471 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e470 - _e471));
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_4 = 0u;
    loop {
        let _e474 = i_4;
        let _e477 = uboFS.lightning.realCountOfLights;
        if (_e474 < _e477) {
            let _e479 = i_4;
            let _e483 = uboFS.lightning.lights[_e479];
            light.position = _e483.position;
            light.color = _e483.color;
            light.attenuation = _e483.attenuation;
            let _e490 = i_4;
            let _e492 = inToLightVectorTangentSpace_arr[_e490];
            let _e493 = inPositionTangentSpace_1;
            toLightVector_3 = (_e492 - _e493);
            let _e495 = toLightVector_3;
            unitToLightVector = normalize(_e495);
            let _e498 = light.attenuation;
            param_101 = _e498.xyz;
            let _e500 = toLightVector_3;
            param_102 = _e500;
            let _e501 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_101), (&param_102));
            attenuationFactor_3 = _e501;
            let _e502 = normal_2;
            param_103 = _e502;
            let _e503 = unitToLightVector;
            param_104 = _e503;
            let _e505 = light.color;
            param_105 = _e505.xyz;
            let _e507 = attenuationFactor_3;
            param_106 = _e507;
            let _e508 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_103), (&param_104), (&param_105), (&param_106));
            let _e509 = totalDiffuse;
            totalDiffuse = (_e509 + _e508);
            let _e511 = normal_2;
            param_107 = _e511;
            let _e512 = unitToLightVector;
            param_108 = _e512;
            let _e513 = unitToCameraVector;
            param_109 = _e513;
            let _e515 = light.color;
            param_110 = _e515.xyz;
            let _e517 = attenuationFactor_3;
            param_111 = _e517;
            let _e518 = shineDamper_1;
            param_112 = _e518;
            let _e519 = reflectivity_1;
            param_113 = _e519;
            let _e520 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_107), (&param_108), (&param_109), (&param_110), (&param_111), (&param_112), (&param_113));
            let _e521 = totalSpecular;
            totalSpecular = (_e521 + _e520);
            continue;
        } else {
            break;
        }
        continuing {
            let _e523 = i_4;
            i_4 = (_e523 + bitcast<u32>(1i));
        }
    }
    let _e526 = totalDiffuse;
    let _e527 = shadow_4;
    let _e531 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e526 * _e527) + vec3(_e531));
    let _e534 = totalSpecular;
    let _e535 = shadow_4;
    totalSpecular = (_e534 * _e535);
    let _e537 = totalDiffuse;
    let _e542 = textureColor;
    let _e544 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e537.x, _e537.y, _e537.z, 1f) * _e542) + vec4<f32>(_e544.x, _e544.y, _e544.z, 0f));
    let _e551 = uboFS.fogColor;
    let _e552 = _e551.xyz;
    let _e557 = baseResultColor;
    let _e558 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e552.x, _e552.y, _e552.z, 1f), _e557, vec4(_e558));
    let _e562 = uboFS.selected;
    if (_e562 != 0u) {
        let _e564 = resultColor;
        let _e566 = uboFS.selectedColor;
        resultColor = mix(_e564, _e566, vec4(0.5f));
    }
    let _e569 = resultColor;
    outColor = _e569;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(7) inToLightVectorTangentSpace0_: vec3<f32>, @location(8) inToLightVectorTangentSpace1_: vec3<f32>, @location(9) inToLightVectorTangentSpace2_: vec3<f32>, @location(10) inToLightVectorTangentSpace3_: vec3<f32>, @location(11) inClipDistance: f32, @location(2) inWorldPosition: vec3<f32>, @location(6) inPositionTangentSpace: vec3<f32>, @location(0) inTextureCoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inViewPosition: vec3<f32>, @location(5) inToCameraVectorTangentSpace: vec3<f32>, @location(4) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVectorTangentSpace0_1 = inToLightVectorTangentSpace0_;
    inToLightVectorTangentSpace1_1 = inToLightVectorTangentSpace1_;
    inToLightVectorTangentSpace2_1 = inToLightVectorTangentSpace2_;
    inToLightVectorTangentSpace3_1 = inToLightVectorTangentSpace3_;
    inClipDistance_1 = inClipDistance;
    inWorldPosition_1 = inWorldPosition;
    inPositionTangentSpace_1 = inPositionTangentSpace;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inViewPosition_1 = inViewPosition;
    inToCameraVectorTangentSpace_1 = inToCameraVectorTangentSpace;
    inVisibility_1 = inVisibility;
    main_1();
    let _e27 = outColor;
    return _e27;
}
