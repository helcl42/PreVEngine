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
    hasNormalMap: u32,
}

var<private> gl_FragCoord_1: vec4<f32>;
var<private> inToLightVectorTangentSpace0_1: vec3<f32>;
var<private> inToLightVectorTangentSpace1_1: vec3<f32>;
var<private> inToLightVectorTangentSpace2_1: vec3<f32>;
var<private> inToLightVectorTangentSpace3_1: vec3<f32>;
var<private> inClipDistance_1: f32;
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
@group(0) @binding(6) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(7) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inWorldPosition_1: vec3<f32>;
@group(0) @binding(4) 
var normalTexture: texture_2d<f32>;
@group(0) @binding(5) 
var normalSampler: sampler;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
@group(0) @binding(2) 
var colorTexture: texture_2d<f32>;
@group(0) @binding(3) 
var colorSampler: sampler;
var<private> inToCameraVectorTangentSpace_1: vec3<f32>;
var<private> inPositionTangentSpace_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e65 = (*toLightVector);
    lightDirection = -(_e65);
    let _e67 = lightDirection;
    let _e68 = (*normal);
    reflectedLightDirection = reflect(_e67, _e68);
    if true {
        let _e70 = (*toLightVector);
        let _e71 = (*toCameraVector);
        halfwayDir = normalize((_e70 + _e71));
        let _e74 = (*normal);
        let _e75 = halfwayDir;
        specularFactor = max(dot(_e74, _e75), 0f);
        let _e78 = (*shineDamper);
        shinePower = (_e78 * 2.4f);
    } else {
        let _e80 = reflectedLightDirection;
        let _e81 = (*toCameraVector);
        specularFactor = max(dot(_e80, _e81), 0f);
        let _e84 = (*shineDamper);
        shinePower = _e84;
    }
    let _e85 = specularFactor;
    let _e86 = shinePower;
    dampedFactor = pow(_e85, _e86);
    let _e88 = dampedFactor;
    let _e89 = (*reflectivity);
    let _e91 = (*lightColor);
    let _e93 = (*attenuationFactor);
    return ((_e91 * (_e88 * _e89)) / vec3(_e93));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e58 = (*normal_1);
    let _e59 = (*toLightVector_1);
    nDotL = dot(_e58, _e59);
    let _e61 = nDotL;
    brightness = max(_e61, 0f);
    let _e63 = brightness;
    let _e64 = (*lightColor_1);
    let _e66 = (*attenuationFactor_1);
    return ((_e64 * _e63) / vec3(_e66));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e56 = (*toLightVector_2);
    toLightDistance = length(_e56);
    let _e59 = (*attenuation)[0u];
    let _e61 = (*attenuation)[1u];
    let _e62 = toLightDistance;
    let _e66 = (*attenuation)[2u];
    let _e67 = toLightDistance;
    let _e69 = toLightDistance;
    attenuationFactor_2 = ((_e59 + (_e61 * _e62)) + ((_e66 * _e67) * _e69));
    let _e72 = attenuationFactor_2;
    return _e72;
}

fn NormalMapping_u0028_t21_u003b_p1_u003b_vf2_u003b(normalMapTexture: texture_2d<f32>, normalMapSampler: sampler, uv: ptr<function, vec2<f32>>) -> vec3<f32> {
    let _e55 = (*uv);
    let _e56 = textureSample(normalMapTexture, normalMapSampler, _e55);
    return normalize(((normalize(_e56.xyz) * 2f) - vec3(1f)));
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_105_: bool;

    shadow = 1f;
    let _e62 = (*shadowCoord)[2u];
    let _e63 = (_e62 >= 0f);
    phi_105_ = _e63;
    if _e63 {
        let _e65 = (*shadowCoord)[2u];
        phi_105_ = (_e65 <= 1f);
    }
    let _e68 = phi_105_;
    if _e68 {
        let _e69 = (*shadowCoord);
        let _e71 = (*shadowCoordOffset);
        let _e72 = (_e69.xy + _e71);
        let _e73 = (*cascadeIndex);
        let _e77 = vec3<f32>(_e72.x, _e72.y, f32(_e73));
        let _e83 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e77.x, _e77.y), i32(_e77.z), 0f);
        depth = _e83.x;
        let _e85 = (*useReverseDepth);
        if (_e85 != 0u) {
            let _e87 = depth;
            let _e89 = (*shadowCoord)[2u];
            let _e90 = (*depthBias);
            if (_e87 > (_e89 + _e90)) {
                shadow = 0.2f;
            }
        } else {
            let _e93 = depth;
            let _e95 = (*shadowCoord)[2u];
            let _e96 = (*depthBias);
            if (_e93 < (_e95 - _e96)) {
                shadow = 0.2f;
            }
        }
    }
    let _e99 = shadow;
    return _e99;
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

    let _e82 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e82).xy);
    let _e86 = textureDim;
    texelSize = (vec2(1f) / _e86);
    shadow_1 = 0f;
    let _e89 = gl_FragCoord_1;
    offset = (fract((_e89.xy * 0.5f)) + vec2(0.25f));
    let _e96 = offset[1u];
    if (_e96 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e99 = texelSize;
    let _e100 = offset;
    let _e103 = (*shadowCoord_1);
    param = _e103;
    param_1 = (_e99 * (_e100 + vec2<f32>(-1.5f, 0.5f)));
    let _e104 = (*cascadeIndex_1);
    param_2 = _e104;
    let _e105 = (*depthBias_1);
    param_3 = _e105;
    let _e106 = (*useReverseDepth_1);
    param_4 = _e106;
    let _e107 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e108 = texelSize;
    let _e109 = offset;
    let _e112 = (*shadowCoord_1);
    param_5 = _e112;
    param_6 = (_e108 * (_e109 + vec2<f32>(0.5f, 0.5f)));
    let _e113 = (*cascadeIndex_1);
    param_7 = _e113;
    let _e114 = (*depthBias_1);
    param_8 = _e114;
    let _e115 = (*useReverseDepth_1);
    param_9 = _e115;
    let _e116 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e118 = texelSize;
    let _e119 = offset;
    let _e122 = (*shadowCoord_1);
    param_10 = _e122;
    param_11 = (_e118 * (_e119 + vec2<f32>(-1.5f, -1.5f)));
    let _e123 = (*cascadeIndex_1);
    param_12 = _e123;
    let _e124 = (*depthBias_1);
    param_13 = _e124;
    let _e125 = (*useReverseDepth_1);
    param_14 = _e125;
    let _e126 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e128 = texelSize;
    let _e129 = offset;
    let _e132 = (*shadowCoord_1);
    param_15 = _e132;
    param_16 = (_e128 * (_e129 + vec2<f32>(0.5f, -1.5f)));
    let _e133 = (*cascadeIndex_1);
    param_17 = _e133;
    let _e134 = (*depthBias_1);
    param_18 = _e134;
    let _e135 = (*useReverseDepth_1);
    param_19 = _e135;
    let _e136 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e107 + _e116) + _e126) + _e136) * 0.25f);
    let _e139 = shadow_1;
    return _e139;
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
        let _e68 = (*shadowCoord_2);
        param_20 = _e68;
        let _e69 = (*cascadeIndex_2);
        param_21 = _e69;
        let _e70 = (*depthBias_2);
        param_22 = _e70;
        let _e71 = (*useReverseDepth_2);
        param_23 = _e71;
        let _e72 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e72;
    } else {
        let _e73 = (*shadowCoord_2);
        param_24 = _e73;
        param_25 = vec2<f32>(0f, 0f);
        let _e74 = (*cascadeIndex_2);
        param_26 = _e74;
        let _e75 = (*depthBias_2);
        param_27 = _e75;
        let _e76 = (*useReverseDepth_2);
        param_28 = _e76;
        let _e77 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e77;
    }
    let _e78 = shadow_2;
    return _e78;
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
    let _e68 = (*depthBias_3);
    bias = _e68;
    let _e70 = (*shadows).enabled;
    if (_e70 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e72 = i;
            if (_e72 < 3u) {
                let _e75 = (*shadows).useReverseDepth;
                if (_e75 != 0u) {
                    let _e78 = (*viewPosition)[2u];
                    let _e79 = i;
                    let _e84 = (*shadows).cascades[_e79].split[0u];
                    if (_e78 > _e84) {
                        let _e86 = i;
                        cascadeIndex_3 = (_e86 + 1u);
                        let _e88 = bias;
                        bias = (_e88 / 1.5f);
                    }
                } else {
                    let _e91 = (*viewPosition)[2u];
                    let _e92 = i;
                    let _e97 = (*shadows).cascades[_e92].split[0u];
                    if (_e91 < _e97) {
                        let _e99 = i;
                        cascadeIndex_3 = (_e99 + 1u);
                        let _e101 = bias;
                        bias = (_e101 / 1.5f);
                    }
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e103 = i;
                i = (_e103 + bitcast<u32>(1i));
            }
        }
        let _e106 = cascadeIndex_3;
        let _e110 = (*shadows).cascades[_e106].viewProjectionMatrix;
        let _e111 = (*worldPosition);
        shadowCoord_3 = (_e110 * vec4<f32>(_e111.x, _e111.y, _e111.z, 1f));
        let _e117 = shadowCoord_3;
        let _e119 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e117 / vec4(_e119));
        let _e122 = normalizedShadowCoord;
        param_29 = _e122;
        let _e123 = cascadeIndex_3;
        param_30 = _e123;
        let _e124 = bias;
        param_31 = _e124;
        let _e126 = (*shadows).useReverseDepth;
        param_32 = _e126;
        let _e127 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e127;
    }
    let _e128 = shadow_3;
    return _e128;
}

fn main_1() {
    var inToLightVectorTangentSpace_arr: array<vec3<f32>, 4>;
    var shadow_4: f32;
    var param_33: Shadows;
    var param_34: vec3<f32>;
    var param_35: vec3<f32>;
    var param_36: f32;
    var normal_2: vec3<f32>;
    var local: vec3<f32>;
    var param_37: vec2<f32>;
    var textureColor: vec4<f32>;
    var unitToCameraVector: vec3<f32>;
    var totalDiffuse: vec3<f32>;
    var totalSpecular: vec3<f32>;
    var i_1: u32;
    var light: Light;
    var toLightVector_3: vec3<f32>;
    var unitToLightVector: vec3<f32>;
    var attenuationFactor_3: f32;
    var param_38: vec3<f32>;
    var param_39: vec3<f32>;
    var param_40: vec3<f32>;
    var param_41: vec3<f32>;
    var param_42: vec3<f32>;
    var param_43: f32;
    var param_44: vec3<f32>;
    var param_45: vec3<f32>;
    var param_46: vec3<f32>;
    var param_47: vec3<f32>;
    var param_48: f32;
    var param_49: f32;
    var param_50: f32;
    var baseResultColor: vec4<f32>;
    var resultColor: vec4<f32>;

    let _e85 = inToLightVectorTangentSpace0_1;
    let _e86 = inToLightVectorTangentSpace1_1;
    let _e87 = inToLightVectorTangentSpace2_1;
    let _e88 = inToLightVectorTangentSpace3_1;
    inToLightVectorTangentSpace_arr = array<vec3<f32>, 4>(_e85, _e86, _e87, _e88);
    let _e90 = inClipDistance_1;
    if (_e90 < 0f) {
        discard;
    }
    shadow_4 = 1f;
    let _e93 = uboFS.castedByShadows;
    if (_e93 != 0u) {
        let _e96 = uboFS.shadows;
        param_33.cascades[0i].viewProjectionMatrix = _e96.cascades[0].viewProjectionMatrix;
        param_33.cascades[0i].split = _e96.cascades[0].split;
        param_33.cascades[1i].viewProjectionMatrix = _e96.cascades[1].viewProjectionMatrix;
        param_33.cascades[1i].split = _e96.cascades[1].split;
        param_33.cascades[2i].viewProjectionMatrix = _e96.cascades[2].viewProjectionMatrix;
        param_33.cascades[2i].split = _e96.cascades[2].split;
        param_33.cascades[3i].viewProjectionMatrix = _e96.cascades[3].viewProjectionMatrix;
        param_33.cascades[3i].split = _e96.cascades[3].split;
        param_33.enabled = _e96.enabled;
        param_33.useReverseDepth = _e96.useReverseDepth;
        let _e127 = inViewPosition_1;
        param_34 = _e127;
        let _e128 = inWorldPosition_1;
        param_35 = _e128;
        param_36 = 0.005f;
        let _e129 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_33), (&param_34), (&param_35), (&param_36));
        shadow_4 = _e129;
    }
    let _e131 = uboFS.hasNormalMap;
    if (_e131 != 0u) {
        let _e133 = inTextureCoord_1;
        param_37 = _e133;
        let _e134 = NormalMapping_u0028_t21_u003b_p1_u003b_vf2_u003b(normalTexture, normalSampler, (&param_37));
        local = _e134;
    } else {
        let _e135 = inNormal_1;
        local = _e135;
    }
    let _e136 = local;
    normal_2 = _e136;
    let _e137 = inTextureCoord_1;
    let _e138 = textureSample(colorTexture, colorSampler, _e137);
    textureColor = _e138;
    let _e139 = inToCameraVectorTangentSpace_1;
    let _e140 = inPositionTangentSpace_1;
    unitToCameraVector = normalize((_e139 - _e140));
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_1 = 0u;
    loop {
        let _e143 = i_1;
        let _e146 = uboFS.lightning.realCountOfLights;
        if (_e143 < _e146) {
            let _e148 = i_1;
            let _e152 = uboFS.lightning.lights[_e148];
            light.position = _e152.position;
            light.color = _e152.color;
            light.attenuation = _e152.attenuation;
            let _e159 = i_1;
            let _e161 = inToLightVectorTangentSpace_arr[_e159];
            let _e162 = inPositionTangentSpace_1;
            toLightVector_3 = (_e161 - _e162);
            let _e164 = toLightVector_3;
            unitToLightVector = normalize(_e164);
            let _e167 = light.attenuation;
            param_38 = _e167.xyz;
            let _e169 = toLightVector_3;
            param_39 = _e169;
            let _e170 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_38), (&param_39));
            attenuationFactor_3 = _e170;
            let _e171 = normal_2;
            param_40 = _e171;
            let _e172 = unitToLightVector;
            param_41 = _e172;
            let _e174 = light.color;
            param_42 = _e174.xyz;
            let _e176 = attenuationFactor_3;
            param_43 = _e176;
            let _e177 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_40), (&param_41), (&param_42), (&param_43));
            let _e178 = totalDiffuse;
            totalDiffuse = (_e178 + _e177);
            let _e180 = normal_2;
            param_44 = _e180;
            let _e181 = unitToLightVector;
            param_45 = _e181;
            let _e182 = unitToCameraVector;
            param_46 = _e182;
            let _e184 = light.color;
            param_47 = _e184.xyz;
            let _e186 = attenuationFactor_3;
            param_48 = _e186;
            let _e189 = uboFS.material.shineDamper;
            param_49 = _e189;
            let _e192 = uboFS.material.reflectivity;
            param_50 = _e192;
            let _e193 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_44), (&param_45), (&param_46), (&param_47), (&param_48), (&param_49), (&param_50));
            let _e194 = totalSpecular;
            totalSpecular = (_e194 + _e193);
            continue;
        } else {
            break;
        }
        continuing {
            let _e196 = i_1;
            i_1 = (_e196 + bitcast<u32>(1i));
        }
    }
    let _e199 = totalDiffuse;
    let _e200 = shadow_4;
    let _e204 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e199 * _e200) + vec3(_e204));
    let _e207 = totalSpecular;
    let _e208 = shadow_4;
    totalSpecular = (_e207 * _e208);
    let _e210 = totalDiffuse;
    let _e215 = textureColor;
    let _e217 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e210.x, _e210.y, _e210.z, 1f) * _e215) + vec4<f32>(_e217.x, _e217.y, _e217.z, 0f));
    let _e224 = uboFS.fogColor;
    let _e225 = _e224.xyz;
    let _e230 = baseResultColor;
    let _e231 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e225.x, _e225.y, _e225.z, 1f), _e230, vec4(_e231));
    let _e235 = uboFS.selected;
    if (_e235 != 0u) {
        let _e237 = resultColor;
        let _e239 = uboFS.selectedColor;
        resultColor = mix(_e237, _e239, vec4(0.5f));
    }
    let _e242 = resultColor;
    outColor = _e242;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(7) inToLightVectorTangentSpace0_: vec3<f32>, @location(8) inToLightVectorTangentSpace1_: vec3<f32>, @location(9) inToLightVectorTangentSpace2_: vec3<f32>, @location(10) inToLightVectorTangentSpace3_: vec3<f32>, @location(11) inClipDistance: f32, @location(3) inViewPosition: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(0) inTextureCoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(5) inToCameraVectorTangentSpace: vec3<f32>, @location(6) inPositionTangentSpace: vec3<f32>, @location(4) inVisibility: f32) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVectorTangentSpace0_1 = inToLightVectorTangentSpace0_;
    inToLightVectorTangentSpace1_1 = inToLightVectorTangentSpace1_;
    inToLightVectorTangentSpace2_1 = inToLightVectorTangentSpace2_;
    inToLightVectorTangentSpace3_1 = inToLightVectorTangentSpace3_;
    inClipDistance_1 = inClipDistance;
    inViewPosition_1 = inViewPosition;
    inWorldPosition_1 = inWorldPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    inToCameraVectorTangentSpace_1 = inToCameraVectorTangentSpace;
    inPositionTangentSpace_1 = inPositionTangentSpace;
    inVisibility_1 = inVisibility;
    main_1();
    let _e27 = outColor;
    return _e27;
}
