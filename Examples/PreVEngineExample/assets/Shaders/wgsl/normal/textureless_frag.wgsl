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
@group(0) @binding(1) 
var<uniform> uboFS: UniformBufferObject;
@group(0) @binding(2) 
var depthTexture_4: texture_2d_array<f32>;
@group(0) @binding(3) 
var depthSampler_4: sampler;
var<private> inViewPosition_1: vec3<f32>;
var<private> inWorldPosition_1: vec3<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inToCameraVector_1: vec3<f32>;
var<private> inVisibility_1: f32;
var<private> outColor: vec4<f32>;
var<private> inTextureCoord_1: vec2<f32>;

fn GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b(normal: ptr<function, vec3<f32>>, toLightVector: ptr<function, vec3<f32>>, toCameraVector: ptr<function, vec3<f32>>, lightColor: ptr<function, vec3<f32>>, attenuationFactor: ptr<function, f32>, shineDamper: ptr<function, f32>, reflectivity: ptr<function, f32>) -> vec3<f32> {
    var lightDirection: vec3<f32>;
    var reflectedLightDirection: vec3<f32>;
    var halfwayDir: vec3<f32>;
    var specularFactor: f32;
    var shinePower: f32;
    var dampedFactor: f32;

    let _e58 = (*toLightVector);
    lightDirection = -(_e58);
    let _e60 = lightDirection;
    let _e61 = (*normal);
    reflectedLightDirection = reflect(_e60, _e61);
    if true {
        let _e63 = (*toLightVector);
        let _e64 = (*toCameraVector);
        halfwayDir = normalize((_e63 + _e64));
        let _e67 = (*normal);
        let _e68 = halfwayDir;
        specularFactor = max(dot(_e67, _e68), 0f);
        let _e71 = (*shineDamper);
        shinePower = (_e71 * 2.4f);
    } else {
        let _e73 = reflectedLightDirection;
        let _e74 = (*toCameraVector);
        specularFactor = max(dot(_e73, _e74), 0f);
        let _e77 = (*shineDamper);
        shinePower = _e77;
    }
    let _e78 = specularFactor;
    let _e79 = shinePower;
    dampedFactor = pow(_e78, _e79);
    let _e81 = dampedFactor;
    let _e82 = (*reflectivity);
    let _e84 = (*lightColor);
    let _e86 = (*attenuationFactor);
    return ((_e84 * (_e81 * _e82)) / vec3(_e86));
}

fn GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b(normal_1: ptr<function, vec3<f32>>, toLightVector_1: ptr<function, vec3<f32>>, lightColor_1: ptr<function, vec3<f32>>, attenuationFactor_1: ptr<function, f32>) -> vec3<f32> {
    var nDotL: f32;
    var brightness: f32;

    let _e51 = (*normal_1);
    let _e52 = (*toLightVector_1);
    nDotL = dot(_e51, _e52);
    let _e54 = nDotL;
    brightness = max(_e54, 0f);
    let _e56 = brightness;
    let _e57 = (*lightColor_1);
    let _e59 = (*attenuationFactor_1);
    return ((_e57 * _e56) / vec3(_e59));
}

fn GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b(attenuation: ptr<function, vec3<f32>>, toLightVector_2: ptr<function, vec3<f32>>) -> f32 {
    var toLightDistance: f32;
    var attenuationFactor_2: f32;

    let _e49 = (*toLightVector_2);
    toLightDistance = length(_e49);
    let _e52 = (*attenuation)[0u];
    let _e54 = (*attenuation)[1u];
    let _e55 = toLightDistance;
    let _e59 = (*attenuation)[2u];
    let _e60 = toLightDistance;
    let _e62 = toLightDistance;
    attenuationFactor_2 = ((_e52 + (_e54 * _e55)) + ((_e59 * _e60) * _e62));
    let _e65 = attenuationFactor_2;
    return _e65;
}

fn GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture: texture_2d_array<f32>, depthSampler: sampler, shadowCoord: ptr<function, vec4<f32>>, shadowCoordOffset: ptr<function, vec2<f32>>, cascadeIndex: ptr<function, u32>, depthBias: ptr<function, f32>, useReverseDepth: ptr<function, u32>) -> f32 {
    var shadow: f32;
    var depth: f32;
    var phi_97_: bool;

    shadow = 1f;
    let _e55 = (*shadowCoord)[2u];
    let _e56 = (_e55 >= 0f);
    phi_97_ = _e56;
    if _e56 {
        let _e58 = (*shadowCoord)[2u];
        phi_97_ = (_e58 <= 1f);
    }
    let _e61 = phi_97_;
    if _e61 {
        let _e62 = (*shadowCoord);
        let _e64 = (*shadowCoordOffset);
        let _e65 = (_e62.xy + _e64);
        let _e66 = (*cascadeIndex);
        let _e70 = vec3<f32>(_e65.x, _e65.y, f32(_e66));
        let _e76 = textureSampleLevel(depthTexture, depthSampler, vec2<f32>(_e70.x, _e70.y), i32(_e70.z), 0f);
        depth = _e76.x;
        let _e78 = (*useReverseDepth);
        if (_e78 != 0u) {
            let _e80 = depth;
            let _e82 = (*shadowCoord)[2u];
            let _e83 = (*depthBias);
            if (_e80 > (_e82 + _e83)) {
                shadow = 0.2f;
            }
        } else {
            let _e86 = depth;
            let _e88 = (*shadowCoord)[2u];
            let _e89 = (*depthBias);
            if (_e86 < (_e88 - _e89)) {
                shadow = 0.2f;
            }
        }
    }
    let _e92 = shadow;
    return _e92;
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

    let _e75 = textureDimensions(depthTexture_1, 0i);
    textureDim = vec2<f32>(vec2<i32>(_e75).xy);
    let _e79 = textureDim;
    texelSize = (vec2(1f) / _e79);
    shadow_1 = 0f;
    let _e82 = gl_FragCoord_1;
    offset = (fract((_e82.xy * 0.5f)) + vec2(0.25f));
    let _e89 = offset[1u];
    if (_e89 > 1.1f) {
        offset[1u] = 0f;
    }
    let _e92 = texelSize;
    let _e93 = offset;
    let _e96 = (*shadowCoord_1);
    param = _e96;
    param_1 = (_e92 * (_e93 + vec2<f32>(-1.5f, 0.5f)));
    let _e97 = (*cascadeIndex_1);
    param_2 = _e97;
    let _e98 = (*depthBias_1);
    param_3 = _e98;
    let _e99 = (*useReverseDepth_1);
    param_4 = _e99;
    let _e100 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param), (&param_1), (&param_2), (&param_3), (&param_4));
    let _e101 = texelSize;
    let _e102 = offset;
    let _e105 = (*shadowCoord_1);
    param_5 = _e105;
    param_6 = (_e101 * (_e102 + vec2<f32>(0.5f, 0.5f)));
    let _e106 = (*cascadeIndex_1);
    param_7 = _e106;
    let _e107 = (*depthBias_1);
    param_8 = _e107;
    let _e108 = (*useReverseDepth_1);
    param_9 = _e108;
    let _e109 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_5), (&param_6), (&param_7), (&param_8), (&param_9));
    let _e111 = texelSize;
    let _e112 = offset;
    let _e115 = (*shadowCoord_1);
    param_10 = _e115;
    param_11 = (_e111 * (_e112 + vec2<f32>(-1.5f, -1.5f)));
    let _e116 = (*cascadeIndex_1);
    param_12 = _e116;
    let _e117 = (*depthBias_1);
    param_13 = _e117;
    let _e118 = (*useReverseDepth_1);
    param_14 = _e118;
    let _e119 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    let _e121 = texelSize;
    let _e122 = offset;
    let _e125 = (*shadowCoord_1);
    param_15 = _e125;
    param_16 = (_e121 * (_e122 + vec2<f32>(0.5f, -1.5f)));
    let _e126 = (*cascadeIndex_1);
    param_17 = _e126;
    let _e127 = (*depthBias_1);
    param_18 = _e127;
    let _e128 = (*useReverseDepth_1);
    param_19 = _e128;
    let _e129 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_1, depthSampler_1, (&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
    shadow_1 = ((((_e100 + _e109) + _e119) + _e129) * 0.25f);
    let _e132 = shadow_1;
    return _e132;
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
        let _e61 = (*shadowCoord_2);
        param_20 = _e61;
        let _e62 = (*cascadeIndex_2);
        param_21 = _e62;
        let _e63 = (*depthBias_2);
        param_22 = _e63;
        let _e64 = (*useReverseDepth_2);
        param_23 = _e64;
        let _e65 = GetShadowPCFInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_20), (&param_21), (&param_22), (&param_23));
        shadow_2 = _e65;
    } else {
        let _e66 = (*shadowCoord_2);
        param_24 = _e66;
        param_25 = vec2<f32>(0f, 0f);
        let _e67 = (*cascadeIndex_2);
        param_26 = _e67;
        let _e68 = (*depthBias_2);
        param_27 = _e68;
        let _e69 = (*useReverseDepth_2);
        param_28 = _e69;
        let _e70 = GetShadowRawInternal_u0028_tA21_u003b_p1_u003b_vf4_u003b_vf2_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_2, depthSampler_2, (&param_24), (&param_25), (&param_26), (&param_27), (&param_28));
        shadow_2 = _e70;
    }
    let _e71 = shadow_2;
    return _e71;
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
    let _e61 = (*depthBias_3);
    bias = _e61;
    let _e63 = (*shadows).enabled;
    if (_e63 != 0u) {
        cascadeIndex_3 = 0u;
        i = 0u;
        loop {
            let _e65 = i;
            if (_e65 < 3u) {
                let _e68 = (*viewPosition)[2u];
                let _e69 = i;
                let _e74 = (*shadows).cascades[_e69].split[0u];
                if (_e68 < _e74) {
                    let _e76 = i;
                    cascadeIndex_3 = (_e76 + 1u);
                    let _e78 = bias;
                    bias = (_e78 / 1.5f);
                }
                continue;
            } else {
                break;
            }
            continuing {
                let _e80 = i;
                i = (_e80 + bitcast<u32>(1i));
            }
        }
        let _e83 = cascadeIndex_3;
        let _e87 = (*shadows).cascades[_e83].viewProjectionMatrix;
        let _e88 = (*worldPosition);
        shadowCoord_3 = (_e87 * vec4<f32>(_e88.x, _e88.y, _e88.z, 1f));
        let _e94 = shadowCoord_3;
        let _e96 = shadowCoord_3[3u];
        normalizedShadowCoord = (_e94 / vec4(_e96));
        let _e99 = normalizedShadowCoord;
        param_29 = _e99;
        let _e100 = cascadeIndex_3;
        param_30 = _e100;
        let _e101 = bias;
        param_31 = _e101;
        let _e103 = (*shadows).useReverseDepth;
        param_32 = _e103;
        let _e104 = GetShadow_u0028_tA21_u003b_p1_u003b_vf4_u003b_u1_u003b_f1_u003b_u1_u003b(depthTexture_3, depthSampler_3, (&param_29), (&param_30), (&param_31), (&param_32));
        shadow_3 = _e104;
    }
    let _e105 = shadow_3;
    return _e105;
}

fn main_1() {
    var inToLightVector_arr: array<vec3<f32>, 4>;
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

    let _e75 = inToLightVector0_1;
    let _e76 = inToLightVector1_1;
    let _e77 = inToLightVector2_1;
    let _e78 = inToLightVector3_1;
    inToLightVector_arr = array<vec3<f32>, 4>(_e75, _e76, _e77, _e78);
    let _e80 = inClipDistance_1;
    if (_e80 < 0f) {
        discard;
    }
    shadow_4 = 1f;
    let _e83 = uboFS.castedByShadows;
    if (_e83 != 0u) {
        let _e86 = uboFS.shadows;
        param_33.cascades[0i].viewProjectionMatrix = _e86.cascades[0].viewProjectionMatrix;
        param_33.cascades[0i].split = _e86.cascades[0].split;
        param_33.cascades[1i].viewProjectionMatrix = _e86.cascades[1].viewProjectionMatrix;
        param_33.cascades[1i].split = _e86.cascades[1].split;
        param_33.cascades[2i].viewProjectionMatrix = _e86.cascades[2].viewProjectionMatrix;
        param_33.cascades[2i].split = _e86.cascades[2].split;
        param_33.cascades[3i].viewProjectionMatrix = _e86.cascades[3].viewProjectionMatrix;
        param_33.cascades[3i].split = _e86.cascades[3].split;
        param_33.enabled = _e86.enabled;
        param_33.useReverseDepth = _e86.useReverseDepth;
        let _e117 = inViewPosition_1;
        param_34 = _e117;
        let _e118 = inWorldPosition_1;
        param_35 = _e118;
        param_36 = 0.02f;
        let _e119 = GetShadow_u0028_tA21_u003b_p1_u003b_struct_u002d_Shadows_u002d_struct_u002d_ShadowsCascade_u002d_mf44_u002d_vf41_u005b_4_u005d_u002d_u1_u002d_u11_u003b_vf3_u003b_vf3_u003b_f1_u003b(depthTexture_4, depthSampler_4, (&param_33), (&param_34), (&param_35), (&param_36));
        shadow_4 = _e119;
    }
    let _e120 = inNormal_1;
    unitNormal = normalize(_e120);
    let _e122 = inToCameraVector_1;
    unitToCameraVector = normalize(_e122);
    totalDiffuse = vec3<f32>(0f, 0f, 0f);
    totalSpecular = vec3<f32>(0f, 0f, 0f);
    i_1 = 0u;
    loop {
        let _e124 = i_1;
        let _e127 = uboFS.lightning.realCountOfLights;
        if (_e124 < _e127) {
            let _e129 = i_1;
            let _e133 = uboFS.lightning.lights[_e129];
            light.position = _e133.position;
            light.color = _e133.color;
            light.attenuation = _e133.attenuation;
            let _e140 = i_1;
            let _e142 = inToLightVector_arr[_e140];
            toLightVector_3 = _e142;
            let _e143 = toLightVector_3;
            unitToLightVector = normalize(_e143);
            let _e146 = light.attenuation;
            param_37 = _e146.xyz;
            let _e148 = toLightVector_3;
            param_38 = _e148;
            let _e149 = GetAttenuationFactor_u0028_vf3_u003b_vf3_u003b((&param_37), (&param_38));
            attenuationFactor_3 = _e149;
            let _e150 = unitNormal;
            param_39 = _e150;
            let _e151 = unitToLightVector;
            param_40 = _e151;
            let _e153 = light.color;
            param_41 = _e153.xyz;
            let _e155 = attenuationFactor_3;
            param_42 = _e155;
            let _e156 = GetDiffuseColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b((&param_39), (&param_40), (&param_41), (&param_42));
            let _e157 = totalDiffuse;
            totalDiffuse = (_e157 + _e156);
            let _e159 = unitNormal;
            param_43 = _e159;
            let _e160 = unitToLightVector;
            param_44 = _e160;
            let _e161 = unitToCameraVector;
            param_45 = _e161;
            let _e163 = light.color;
            param_46 = _e163.xyz;
            let _e165 = attenuationFactor_3;
            param_47 = _e165;
            let _e168 = uboFS.material.shineDamper;
            param_48 = _e168;
            let _e171 = uboFS.material.reflectivity;
            param_49 = _e171;
            let _e172 = GetSpecularColor_u0028_vf3_u003b_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_f1_u003b_f1_u003b((&param_43), (&param_44), (&param_45), (&param_46), (&param_47), (&param_48), (&param_49));
            let _e173 = totalSpecular;
            totalSpecular = (_e173 + _e172);
            continue;
        } else {
            break;
        }
        continuing {
            let _e175 = i_1;
            i_1 = (_e175 + bitcast<u32>(1i));
        }
    }
    let _e178 = totalDiffuse;
    let _e179 = shadow_4;
    let _e183 = uboFS.lightning.ambientFactor;
    totalDiffuse = ((_e178 * _e179) + vec3(_e183));
    let _e186 = totalSpecular;
    let _e187 = shadow_4;
    totalSpecular = (_e186 * _e187);
    let _e189 = totalDiffuse;
    let _e196 = uboFS.material.color;
    let _e198 = totalSpecular;
    baseResultColor = ((vec4<f32>(_e189.x, _e189.y, _e189.z, 1f) * _e196) + vec4<f32>(_e198.x, _e198.y, _e198.z, 0f));
    let _e205 = uboFS.fogColor;
    let _e206 = _e205.xyz;
    let _e211 = baseResultColor;
    let _e212 = inVisibility_1;
    resultColor = mix(vec4<f32>(_e206.x, _e206.y, _e206.z, 1f), _e211, vec4(_e212));
    let _e216 = uboFS.selected;
    if (_e216 != 0u) {
        let _e218 = resultColor;
        let _e220 = uboFS.selectedColor;
        resultColor = mix(_e218, _e220, vec4(0.5f));
    }
    let _e223 = resultColor;
    outColor = _e223;
    return;
}

@fragment 
fn main(@builtin(position) gl_FragCoord: vec4<f32>, @location(6) inToLightVector0_: vec3<f32>, @location(7) inToLightVector1_: vec3<f32>, @location(8) inToLightVector2_: vec3<f32>, @location(9) inToLightVector3_: vec3<f32>, @location(10) inClipDistance: f32, @location(3) inViewPosition: vec3<f32>, @location(2) inWorldPosition: vec3<f32>, @location(1) inNormal: vec3<f32>, @location(4) inToCameraVector: vec3<f32>, @location(5) inVisibility: f32, @location(0) inTextureCoord: vec2<f32>) -> @location(0) vec4<f32> {
    gl_FragCoord_1 = gl_FragCoord;
    inToLightVector0_1 = inToLightVector0_;
    inToLightVector1_1 = inToLightVector1_;
    inToLightVector2_1 = inToLightVector2_;
    inToLightVector3_1 = inToLightVector3_;
    inClipDistance_1 = inClipDistance;
    inViewPosition_1 = inViewPosition;
    inWorldPosition_1 = inWorldPosition;
    inNormal_1 = inNormal;
    inToCameraVector_1 = inToCameraVector;
    inVisibility_1 = inVisibility;
    inTextureCoord_1 = inTextureCoord;
    main_1();
    let _e25 = outColor;
    return _e25;
}
