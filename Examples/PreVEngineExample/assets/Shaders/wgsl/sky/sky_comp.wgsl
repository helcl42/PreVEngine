struct RaymarchToCloudResult {
    hit: bool,
    position: vec3<f32>,
    color: vec4<f32>,
}

struct UniformBufferObject {
    resolution: vec4<f32>,
    projectionMatrix: mat4x4<f32>,
    inverseProjectionMatrix: mat4x4<f32>,
    viewMatrix: mat4x4<f32>,
    inverseViewMatrix: mat4x4<f32>,
    lightColor: vec4<f32>,
    lightDirection: vec4<f32>,
    cameraPosition: vec4<f32>,
    baseCloudColor: vec4<f32>,
    skyColorBottom: vec4<f32>,
    skyColorTop: vec4<f32>,
    windDirection: vec4<f32>,
    worldOrigin: vec4<f32>,
    time: f32,
    coverageFactor: f32,
    cloudSpeed: f32,
    crispiness: f32,
    absorption: f32,
    curliness: f32,
    enablePowder: u32,
    densityFactor: f32,
    earthRadius: f32,
    sphereInnerRadius: f32,
    sphereOuterRadius: f32,
    cloudTopOffset: f32,
    maxDepth: f32,
}

@group(0) @binding(8) 
var<uniform> uboCS: UniformBufferObject;
@group(0) @binding(4) 
var perlinNoiseTex: texture_3d<f32>;
@group(0) @binding(5) 
var perlinNoiseSampler: sampler;
@group(0) @binding(6) 
var weatherTex: texture_2d<f32>;
@group(0) @binding(7) 
var weatherSampler: sampler;
var<private> gl_GlobalInvocationID_1: vec3<u32>;
@group(0) @binding(0) 
var outFragColor: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(1) 
var outBloom: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(2) 
var outAlphaness: texture_storage_2d<rgba8unorm,write>;
@group(0) @binding(3) 
var outCloudDistance: texture_storage_2d<r32float,write>;

fn ComputeDepth_u0028_mf44_u003b_mf44_u003b_vf3_u003b(projectionMatrix: ptr<function, mat4x4<f32>>, viewMatrix: ptr<function, mat4x4<f32>>, pointInWorldSpace: ptr<function, vec3<f32>>) -> f32 {
    var pInClipSpace: vec4<f32>;
    var pInNDC: vec3<f32>;
    var depth: f32;

    let _e143 = (*projectionMatrix);
    let _e144 = (*viewMatrix);
    let _e146 = (*pointInWorldSpace);
    pInClipSpace = ((_e143 * _e144) * vec4<f32>(_e146.x, _e146.y, _e146.z, 1f));
    let _e152 = pInClipSpace;
    let _e155 = pInClipSpace[3u];
    pInNDC = (_e152.xyz / vec3(_e155));
    let _e159 = pInNDC[2u];
    depth = _e159;
    let _e160 = depth;
    return _e160;
}

fn GetSunColor_u0028_vf3_u003b_f1_u003b(worldRayDirection: ptr<function, vec3<f32>>, powExp: ptr<function, f32>) -> vec3<f32> {
    var sun: f32;
    var col: vec3<f32>;

    let _e142 = uboCS.lightDirection;
    let _e144 = (*worldRayDirection);
    sun = clamp(dot(_e142.xyz, _e144), 0f, 1f);
    let _e147 = sun;
    let _e148 = (*powExp);
    col = (vec3<f32>(0.8f, 0.48f, 0.08f) * pow(_e147, _e148));
    let _e151 = col;
    return _e151;
}

fn GetPowder_u0028_f1_u003b(density: ptr<function, f32>) -> f32 {
    let _e138 = (*density);
    return (1f - exp((-2f * _e138)));
}

fn HG_u0028_f1_u003b_f1_u003b(sunDotRayDirection: ptr<function, f32>, g: ptr<function, f32>) -> f32 {
    var gg: f32;

    let _e140 = (*g);
    let _e141 = (*g);
    gg = (_e140 * _e141);
    let _e143 = gg;
    let _e145 = gg;
    let _e147 = (*g);
    let _e149 = (*sunDotRayDirection);
    return ((1f - _e143) / pow(((1f + _e145) - ((2f * _e147) * _e149)), 1.5f));
}

fn GetDensityForCloud_u0028_f1_u003b_f1_u003b(heightFraction: ptr<function, f32>, cloudType: ptr<function, f32>) -> f32 {
    var stratusFactor: f32;
    var stratoCumulusFactor: f32;
    var cumulusFactor: f32;
    var baseGradient: vec4<f32>;

    let _e143 = (*cloudType);
    stratusFactor = (1f - clamp((_e143 * 2f), 0f, 1f));
    let _e147 = (*cloudType);
    stratoCumulusFactor = (1f - (abs((_e147 - 0.5f)) * 2f));
    let _e152 = (*cloudType);
    cumulusFactor = (clamp((_e152 - 0.5f), 0f, 1f) * 2f);
    let _e156 = stratusFactor;
    let _e158 = stratoCumulusFactor;
    let _e161 = cumulusFactor;
    baseGradient = (((vec4<f32>(0f, 0.1f, 0.2f, 0.3f) * _e156) + (vec4<f32>(0.02f, 0.2f, 0.48f, 0.625f) * _e158)) + (vec4<f32>(0f, 0.1625f, 0.88f, 0.98f) * _e161));
    let _e165 = baseGradient[0u];
    let _e167 = baseGradient[1u];
    let _e168 = (*heightFraction);
    let _e171 = baseGradient[2u];
    let _e173 = baseGradient[3u];
    let _e174 = (*heightFraction);
    return (smoothstep(_e165, _e167, _e168) - smoothstep(_e171, _e173, _e174));
}

fn Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b(originalValue: ptr<function, f32>, originalMin: ptr<function, f32>, originalMax: ptr<function, f32>, newMin: ptr<function, f32>, newMax: ptr<function, f32>) -> f32 {
    let _e142 = (*newMin);
    let _e143 = (*originalValue);
    let _e144 = (*originalMin);
    let _e146 = (*originalMax);
    let _e147 = (*originalMin);
    let _e150 = (*newMax);
    let _e151 = (*newMin);
    return (_e142 + (((_e143 - _e144) / (_e146 - _e147)) * (_e150 - _e151)));
}

fn GetUVProjection_u0028_vf3_u003b(inPos: ptr<function, vec3<f32>>) -> vec2<f32> {
    let _e138 = (*inPos);
    let _e141 = uboCS.sphereInnerRadius;
    return ((_e138.xz / vec2(_e141)) + vec2(0.5f));
}

fn GetEarthCenter_u0028_() -> vec3<f32> {
    var sphereCenter: vec3<f32>;

    let _e139 = uboCS.worldOrigin;
    let _e142 = uboCS.earthRadius;
    sphereCenter = (_e139.xyz + vec3<f32>(0f, -(_e142), 0f));
    let _e146 = sphereCenter;
    return _e146;
}

fn GetHeightFraction_u0028_vf3_u003b(inPos_1: ptr<function, vec3<f32>>) -> f32 {
    var sphereCenter_1: vec3<f32>;

    let _e139 = GetEarthCenter_u0028_();
    sphereCenter_1 = _e139;
    let _e140 = (*inPos_1);
    let _e141 = sphereCenter_1;
    let _e145 = uboCS.sphereInnerRadius;
    let _e148 = uboCS.sphereOuterRadius;
    let _e150 = uboCS.sphereInnerRadius;
    return ((length((_e140 - _e141)) - _e145) / (_e148 - _e150));
}

fn SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b(inPos_2: ptr<function, vec3<f32>>, expensive: ptr<function, bool>, lod: ptr<function, f32>) -> f32 {
    var heightFraction_1: f32;
    var param: vec3<f32>;
    var animation: vec3<f32>;
    var uv: vec2<f32>;
    var param_1: vec3<f32>;
    var movingUV: vec2<f32>;
    var param_2: vec3<f32>;
    var lowFrequencyNoise: vec4<f32>;
    var lowFreqFBM: f32;
    var baseCloud: f32;
    var param_3: f32;
    var param_4: f32;
    var param_5: f32;
    var param_6: f32;
    var param_7: f32;
    var density_1: f32;
    var param_8: f32;
    var param_9: f32;
    var weatherData: vec3<f32>;
    var cloudCoverage: f32;
    var baseCloudWithCoverage: f32;
    var param_10: f32;
    var param_11: f32;
    var param_12: f32;
    var param_13: f32;
    var param_14: f32;
    var erodeCloudNoise: vec3<f32>;
    var highFreqFBM: f32;
    var highFreqNoiseModifier: f32;
    var param_15: f32;
    var param_16: f32;
    var param_17: f32;
    var param_18: f32;
    var param_19: f32;

    let _e174 = (*inPos_2);
    param = _e174;
    let _e175 = GetHeightFraction_u0028_vf3_u003b((&param));
    heightFraction_1 = _e175;
    let _e176 = heightFraction_1;
    let _e178 = uboCS.windDirection;
    let _e182 = uboCS.cloudTopOffset;
    let _e185 = uboCS.windDirection;
    let _e188 = uboCS.time;
    let _e191 = uboCS.cloudSpeed;
    animation = (((_e178.xyz * _e176) * _e182) + ((_e185.xyz * _e188) * _e191));
    let _e194 = (*inPos_2);
    param_1 = _e194;
    let _e195 = GetUVProjection_u0028_vf3_u003b((&param_1));
    uv = _e195;
    let _e196 = (*inPos_2);
    let _e197 = animation;
    param_2 = (_e196 + _e197);
    let _e199 = GetUVProjection_u0028_vf3_u003b((&param_2));
    movingUV = _e199;
    let _e200 = heightFraction_1;
    let _e202 = heightFraction_1;
    if ((_e200 < 0f) || (_e202 > 1f)) {
        return 0f;
    }
    let _e205 = uv;
    let _e207 = uboCS.crispiness;
    let _e208 = (_e205 * _e207);
    let _e209 = heightFraction_1;
    let _e213 = (*lod);
    let _e214 = textureSampleLevel(perlinNoiseTex, perlinNoiseSampler, vec3<f32>(_e208.x, _e208.y, _e209), _e213);
    lowFrequencyNoise = _e214;
    let _e215 = lowFrequencyNoise;
    lowFreqFBM = dot(_e215.yzw, vec3<f32>(0.625f, 0.25f, 0.125f));
    let _e218 = lowFreqFBM;
    let _e222 = lowFrequencyNoise[0u];
    param_3 = _e222;
    param_4 = -((1f - _e218));
    param_5 = 1f;
    param_6 = 0f;
    param_7 = 1f;
    let _e223 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_3), (&param_4), (&param_5), (&param_6), (&param_7));
    baseCloud = _e223;
    let _e224 = heightFraction_1;
    param_8 = _e224;
    param_9 = 1f;
    let _e225 = GetDensityForCloud_u0028_f1_u003b_f1_u003b((&param_8), (&param_9));
    density_1 = _e225;
    let _e226 = density_1;
    let _e227 = heightFraction_1;
    let _e230 = baseCloud;
    baseCloud = (_e230 * (_e226 / max(_e227, 0.001f)));
    let _e232 = movingUV;
    let _e233 = textureSampleLevel(weatherTex, weatherSampler, _e232, 0f);
    weatherData = _e233.xyz;
    let _e236 = weatherData[0u];
    let _e238 = uboCS.coverageFactor;
    cloudCoverage = (_e236 * _e238);
    let _e240 = baseCloud;
    param_10 = _e240;
    let _e241 = cloudCoverage;
    param_11 = _e241;
    param_12 = 1f;
    param_13 = 0f;
    param_14 = 1f;
    let _e242 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_10), (&param_11), (&param_12), (&param_13), (&param_14));
    baseCloudWithCoverage = _e242;
    let _e243 = cloudCoverage;
    let _e244 = baseCloudWithCoverage;
    baseCloudWithCoverage = (_e244 * _e243);
    let _e246 = (*expensive);
    if _e246 {
        let _e247 = movingUV;
        let _e249 = uboCS.crispiness;
        let _e250 = (_e247 * _e249);
        let _e251 = heightFraction_1;
        let _e256 = uboCS.curliness;
        let _e258 = (*lod);
        let _e259 = textureSampleLevel(perlinNoiseTex, perlinNoiseSampler, (vec3<f32>(_e250.x, _e250.y, _e251) * _e256), _e258);
        erodeCloudNoise = _e259.xyz;
        let _e261 = erodeCloudNoise;
        highFreqFBM = dot(_e261, vec3<f32>(0.625f, 0.25f, 0.125f));
        let _e263 = highFreqFBM;
        let _e264 = highFreqFBM;
        let _e266 = heightFraction_1;
        highFreqNoiseModifier = mix(_e263, (1f - _e264), clamp((_e266 * 10f), 0f, 1f));
        let _e270 = baseCloudWithCoverage;
        let _e271 = highFreqNoiseModifier;
        let _e272 = baseCloudWithCoverage;
        baseCloudWithCoverage = (_e270 - (_e271 * (1f - _e272)));
        let _e276 = baseCloudWithCoverage;
        let _e278 = highFreqNoiseModifier;
        param_15 = (_e276 * 2f);
        param_16 = (_e278 * 0.2f);
        param_17 = 1f;
        param_18 = 0f;
        param_19 = 1f;
        let _e280 = Remap_u0028_f1_u003b_f1_u003b_f1_u003b_f1_u003b_f1_u003b((&param_15), (&param_16), (&param_17), (&param_18), (&param_19));
        baseCloudWithCoverage = _e280;
    }
    let _e281 = baseCloudWithCoverage;
    return clamp(_e281, 0f, 1f);
}

fn RaymarchToLight_u0028_vf3_u003b_f1_u003b_vf3_u003b_f1_u003b_f1_u003b(inPos_3: ptr<function, vec3<f32>>, stepSize: ptr<function, f32>, lightDir: ptr<function, vec3<f32>>, originalDensity: ptr<function, f32>, lightDotEye: ptr<function, f32>) -> f32 {
    var ds: f32;
    var rayStep: vec3<f32>;
    var sigmaDs: f32;
    var coneRadius: f32;
    var density_2: f32;
    var startPos: vec3<f32>;
    var T: f32;
    var i: i32;
    var pos: vec3<f32>;
    var indexable: array<vec3<f32>, 6>;
    var heightFraction_2: f32;
    var param_20: vec3<f32>;
    var cloudDensity: f32;
    var param_21: vec3<f32>;
    var param_22: bool;
    var param_23: f32;
    var Ti: f32;

    let _e159 = (*stepSize);
    ds = (_e159 * 6f);
    let _e161 = (*lightDir);
    let _e162 = ds;
    rayStep = (_e161 * _e162);
    let _e164 = ds;
    let _e167 = uboCS.absorption;
    sigmaDs = (-(_e164) * _e167);
    coneRadius = 1f;
    density_2 = 0f;
    let _e169 = (*inPos_3);
    startPos = _e169;
    T = 1f;
    i = 0i;
    loop {
        let _e170 = i;
        if (_e170 < 6i) {
            let _e172 = startPos;
            let _e173 = coneRadius;
            let _e174 = i;
            indexable = array<vec3<f32>, 6>(vec3<f32>(0.38051304f, 0.9245345f, -0.02111345f), vec3<f32>(-0.506258f, -0.03590792f, -0.8616342f), vec3<f32>(-0.32509217f, -0.9455744f, 0.01428793f), vec3<f32>(0.09026238f, -0.27376544f, 0.95755166f), vec3<f32>(0.28128597f, 0.4244364f, -0.8606579f), vec3<f32>(-0.16852403f, 0.14748697f, 0.97460103f));
            let _e176 = indexable[_e174];
            let _e178 = i;
            pos = (_e172 + ((_e176 * _e173) * f32(_e178)));
            let _e182 = pos;
            param_20 = _e182;
            let _e183 = GetHeightFraction_u0028_vf3_u003b((&param_20));
            heightFraction_2 = _e183;
            let _e184 = heightFraction_2;
            if (_e184 >= 0f) {
                let _e186 = density_2;
                let _e188 = i;
                let _e191 = pos;
                param_21 = _e191;
                param_22 = (_e186 > 0.3f);
                param_23 = f32((_e188 / 16i));
                let _e192 = SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b((&param_21), (&param_22), (&param_23));
                cloudDensity = _e192;
                let _e193 = cloudDensity;
                if (_e193 > 0f) {
                    let _e195 = cloudDensity;
                    let _e196 = sigmaDs;
                    Ti = exp((_e195 * _e196));
                    let _e199 = Ti;
                    let _e200 = T;
                    T = (_e200 * _e199);
                    let _e202 = cloudDensity;
                    let _e203 = density_2;
                    density_2 = (_e203 + _e202);
                }
            }
            let _e205 = rayStep;
            let _e206 = startPos;
            startPos = (_e206 + _e205);
            let _e208 = coneRadius;
            coneRadius = (_e208 + 0.16666667f);
            continue;
        } else {
            break;
        }
        continuing {
            let _e210 = i;
            i = (_e210 + 1i);
        }
    }
    let _e212 = T;
    return _e212;
}

fn CreateDefaultRaymarchToCloudResult_u0028_() -> RaymarchToCloudResult {
    var result: RaymarchToCloudResult;

    result.hit = false;
    result.position = vec3<f32>(1000000000000f, 1000000000000f, 1000000000000f);
    result.color = vec4<f32>(0f, 0f, 0f, 0f);
    let _e141 = result;
    return _e141;
}

fn RaymarchToCloud_u0028_vf3_u003b_vf3_u003b_vf3_u003b(startPos_1: ptr<function, vec3<f32>>, endPos: ptr<function, vec3<f32>>, bg: ptr<function, vec3<f32>>) -> RaymarchToCloudResult {
    var path: vec3<f32>;
    var pathLength: f32;
    var step_: f32;
    var dir: vec3<f32>;
    var stepVector: vec3<f32>;
    var fragCoord: vec2<u32>;
    var a: i32;
    var b: i32;
    var pos_1: vec3<f32>;
    var indexable_1: array<f32, 16>;
    var density_3: f32;
    var lightDotEye_1: f32;
    var T_1: f32;
    var sigmaDs_1: f32;
    var result_1: RaymarchToCloudResult;
    var i_1: i32;
    var densitySample: f32;
    var param_24: vec3<f32>;
    var param_25: bool;
    var param_26: f32;
    var ambientLight: vec3<f32>;
    var lightDensity: f32;
    var param_27: vec3<f32>;
    var param_28: f32;
    var param_29: vec3<f32>;
    var param_30: f32;
    var param_31: f32;
    var scattering: f32;
    var param_32: f32;
    var param_33: f32;
    var param_34: f32;
    var param_35: f32;
    var powderTerm: f32;
    var param_36: f32;
    var S: vec3<f32>;
    var dTrans: f32;
    var Sint: vec3<f32>;

    let _e177 = (*endPos);
    let _e178 = (*startPos_1);
    path = (_e177 - _e178);
    let _e180 = path;
    pathLength = length(_e180);
    let _e182 = pathLength;
    step_ = (_e182 / 64f);
    let _e184 = path;
    dir = normalize(_e184);
    let _e186 = dir;
    let _e187 = step_;
    stepVector = (_e186 * _e187);
    let _e189 = gl_GlobalInvocationID_1;
    fragCoord = _e189.xy;
    let _e192 = fragCoord[0u];
    let _e193 = bitcast<i32>(_e192);
    a = (_e193 - (i32(floor((f32(_e193) / f32(4i)))) * 4i));
    let _e202 = fragCoord[1u];
    let _e203 = bitcast<i32>(_e202);
    b = (_e203 - (i32(floor((f32(_e203) / f32(4i)))) * 4i));
    let _e211 = (*startPos_1);
    let _e212 = stepVector;
    let _e213 = a;
    let _e215 = b;
    indexable_1 = array<f32, 16>(0f, 0.5f, 0.125f, 0.625f, 0.75f, 0.25f, 0.875f, 0.375f, 0.1875f, 0.6875f, 0.0625f, 0.5625f, 0.9375f, 0.4375f, 0.8125f, 0.3125f);
    let _e218 = indexable_1[((_e213 * 4i) + _e215)];
    pos_1 = (_e211 + (_e212 * _e218));
    density_3 = 0f;
    let _e222 = uboCS.lightDirection;
    let _e225 = dir;
    lightDotEye_1 = dot(normalize(_e222.xyz), _e225);
    T_1 = 1f;
    let _e227 = step_;
    let _e230 = uboCS.densityFactor;
    sigmaDs_1 = (-(_e227) * _e230);
    let _e232 = CreateDefaultRaymarchToCloudResult_u0028_();
    result_1 = _e232;
    i_1 = 0i;
    loop {
        let _e233 = i_1;
        if (_e233 < 64i) {
            let _e235 = i_1;
            let _e238 = pos_1;
            param_24 = _e238;
            param_25 = true;
            param_26 = f32((_e235 / 16i));
            let _e239 = SampleCloudDensity_u0028_vf3_u003b_b1_u003b_f1_u003b((&param_24), (&param_25), (&param_26));
            densitySample = _e239;
            let _e240 = densitySample;
            if (_e240 > 0f) {
                let _e243 = result_1.hit;
                if !(_e243) {
                    let _e245 = pos_1;
                    result_1.position = _e245;
                    result_1.hit = true;
                }
                let _e249 = uboCS.baseCloudColor;
                ambientLight = _e249.xyz;
                let _e251 = step_;
                let _e253 = pos_1;
                param_27 = _e253;
                param_28 = (_e251 * 0.1f);
                let _e255 = uboCS.lightDirection;
                param_29 = _e255.xyz;
                let _e257 = densitySample;
                param_30 = _e257;
                let _e258 = lightDotEye_1;
                param_31 = _e258;
                let _e259 = RaymarchToLight_u0028_vf3_u003b_f1_u003b_vf3_u003b_f1_u003b_f1_u003b((&param_27), (&param_28), (&param_29), (&param_30), (&param_31));
                lightDensity = _e259;
                let _e260 = lightDotEye_1;
                param_32 = _e260;
                param_33 = -0.08f;
                let _e261 = HG_u0028_f1_u003b_f1_u003b((&param_32), (&param_33));
                let _e262 = lightDotEye_1;
                param_34 = _e262;
                param_35 = 0.08f;
                let _e263 = HG_u0028_f1_u003b_f1_u003b((&param_34), (&param_35));
                let _e264 = lightDotEye_1;
                scattering = mix(_e261, _e263, clamp(((_e264 * 0.5f) + 0.5f), 0f, 1f));
                let _e269 = scattering;
                scattering = max(_e269, 1f);
                powderTerm = 1f;
                let _e272 = uboCS.enablePowder;
                if (_e272 == 1u) {
                    let _e274 = densitySample;
                    param_36 = _e274;
                    let _e275 = GetPowder_u0028_f1_u003b((&param_36));
                    powderTerm = _e275;
                }
                let _e276 = ambientLight;
                let _e278 = (*bg);
                let _e281 = scattering;
                let _e283 = uboCS.lightColor;
                let _e286 = powderTerm;
                let _e287 = lightDensity;
                let _e292 = densitySample;
                S = ((mix(mix((_e276 * 1.8f), _e278, vec3(0.2f)), (_e283.xyz * _e281), vec3((_e286 * _e287))) * 0.6f) * _e292);
                let _e294 = densitySample;
                let _e295 = sigmaDs_1;
                dTrans = exp((_e294 * _e295));
                let _e298 = S;
                let _e299 = S;
                let _e300 = dTrans;
                let _e303 = densitySample;
                Sint = ((_e298 - (_e299 * _e300)) * (1f / _e303));
                let _e306 = T_1;
                let _e307 = Sint;
                let _e310 = result_1.color;
                let _e312 = (_e310.xyz + (_e307 * _e306));
                result_1.color[0u] = _e312.x;
                result_1.color[1u] = _e312.y;
                result_1.color[2u] = _e312.z;
                let _e322 = dTrans;
                let _e323 = T_1;
                T_1 = (_e323 * _e322);
            }
            let _e325 = T_1;
            if (_e325 <= 0.1f) {
                break;
            }
            let _e327 = stepVector;
            let _e328 = pos_1;
            pos_1 = (_e328 + _e327);
            continue;
        } else {
            break;
        }
        continuing {
            let _e330 = i_1;
            i_1 = (_e330 + 1i);
        }
    }
    let _e332 = T_1;
    result_1.color[3u] = (1f - _e332);
    let _e336 = result_1;
    return _e336;
}

fn ComputeFogAmount_u0028_vf3_u003b_f1_u003b(startPos_2: ptr<function, vec3<f32>>, factor: ptr<function, f32>) -> f32 {
    var sphereCenter_2: vec3<f32>;
    var dist: f32;
    var radius: f32;
    var alpha: f32;

    let _e143 = GetEarthCenter_u0028_();
    sphereCenter_2 = _e143;
    let _e144 = (*startPos_2);
    let _e146 = uboCS.cameraPosition;
    dist = length((_e144 - _e146.xyz));
    let _e152 = uboCS.cameraPosition[1u];
    let _e154 = sphereCenter_2[1u];
    radius = ((_e152 - _e154) * 0.3f);
    let _e157 = dist;
    let _e158 = radius;
    alpha = (_e157 / _e158);
    let _e160 = dist;
    let _e162 = alpha;
    let _e164 = (*factor);
    return (1f - exp(((-(_e160) * _e162) * _e164)));
}

fn RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b(rayStartPosition: ptr<function, vec3<f32>>, rayDirection: ptr<function, vec3<f32>>, sphereCenter_3: ptr<function, vec3<f32>>, radius_1: ptr<function, f32>, intersectionPoint: ptr<function, vec3<f32>>) -> bool {
    var radius2_: f32;
    var L: vec3<f32>;
    var a_1: f32;
    var b_1: f32;
    var c: f32;
    var discr: f32;
    var sqrtDiscr: f32;
    var t1_: f32;
    var t2_: f32;
    var t: f32;

    let _e152 = (*radius_1);
    let _e153 = (*radius_1);
    radius2_ = (_e152 * _e153);
    let _e155 = (*rayStartPosition);
    let _e156 = (*sphereCenter_3);
    L = (_e155 - _e156);
    let _e158 = (*rayDirection);
    let _e159 = (*rayDirection);
    a_1 = dot(_e158, _e159);
    let _e161 = (*rayDirection);
    let _e162 = L;
    b_1 = (2f * dot(_e161, _e162));
    let _e165 = L;
    let _e166 = L;
    let _e168 = radius2_;
    c = (dot(_e165, _e166) - _e168);
    let _e170 = b_1;
    let _e171 = b_1;
    let _e173 = a_1;
    let _e175 = c;
    discr = ((_e170 * _e171) - ((4f * _e173) * _e175));
    let _e178 = discr;
    if (_e178 < 0f) {
        return false;
    }
    let _e180 = discr;
    sqrtDiscr = sqrt(_e180);
    let _e182 = b_1;
    let _e184 = sqrtDiscr;
    let _e186 = a_1;
    t1_ = ((-(_e182) - _e184) / (2f * _e186));
    let _e189 = b_1;
    let _e191 = sqrtDiscr;
    let _e193 = a_1;
    t2_ = ((-(_e189) + _e191) / (2f * _e193));
    let _e196 = t1_;
    let _e198 = t1_;
    let _e199 = t2_;
    t = select(_e199, _e198, (_e196 > 0f));
    let _e201 = t;
    if (_e201 <= 0f) {
        return false;
    }
    let _e203 = (*rayStartPosition);
    let _e204 = (*rayDirection);
    let _e205 = t;
    (*intersectionPoint) = (_e203 + (_e204 * _e205));
    return true;
}

fn GetCubeMapColor_u0028_vf3_u003b(worldRayDirection_1: ptr<function, vec3<f32>>) -> vec3<f32> {
    var transitionRatio: f32;
    var color: vec3<f32>;

    let _e141 = (*worldRayDirection_1)[1u];
    transitionRatio = smoothstep(-0.07f, 0.07f, _e141);
    let _e144 = uboCS.skyColorBottom;
    let _e147 = uboCS.skyColorTop;
    let _e149 = transitionRatio;
    color = mix(_e144.xyz, _e147.xyz, vec3(_e149));
    let _e152 = color;
    return _e152;
}

fn GetBackgroundColor_u0028_vf3_u003b(worldRayDirection_2: ptr<function, vec3<f32>>) -> vec4<f32> {
    var backgroundColor: vec3<f32>;
    var param_37: vec3<f32>;
    var sunColor: vec3<f32>;
    var param_38: vec3<f32>;
    var param_39: f32;
    var finalColor: vec3<f32>;

    let _e144 = (*worldRayDirection_2);
    param_37 = _e144;
    let _e145 = GetCubeMapColor_u0028_vf3_u003b((&param_37));
    backgroundColor = _e145;
    let _e146 = (*worldRayDirection_2);
    param_38 = _e146;
    param_39 = 350f;
    let _e147 = GetSunColor_u0028_vf3_u003b_f1_u003b((&param_38), (&param_39));
    sunColor = _e147;
    let _e148 = backgroundColor;
    let _e149 = sunColor;
    finalColor = (_e148 + _e149);
    let _e151 = finalColor;
    return vec4<f32>(_e151.x, _e151.y, _e151.z, 1f);
}

fn ComputeWorldSpaceDirection_u0028_vf4_u003b(viewSpaceDir: ptr<function, vec4<f32>>) -> vec3<f32> {
    var worldDir: vec3<f32>;

    let _e140 = uboCS.inverseViewMatrix;
    let _e141 = (*viewSpaceDir);
    worldDir = (_e140 * _e141).xyz;
    let _e144 = worldDir;
    return normalize(_e144);
}

fn ComputeViewSpaceDirection_u0028_vf4_u003b(clipSpaceDir: ptr<function, vec4<f32>>) -> vec4<f32> {
    var rayView: vec4<f32>;

    let _e140 = uboCS.inverseProjectionMatrix;
    let _e141 = (*clipSpaceDir);
    rayView = (_e140 * _e141);
    let _e143 = rayView;
    let _e144 = _e143.xy;
    return vec4<f32>(_e144.x, _e144.y, -1f, 0f);
}

fn ComputeClipSpaceDirection_u0028_vi2_u003b(fragCoord_1: ptr<function, vec2<i32>>) -> vec4<f32> {
    var normalizedFragCoord: vec2<f32>;
    var rayNds: vec2<f32>;

    let _e140 = (*fragCoord_1);
    let _e143 = uboCS.resolution;
    normalizedFragCoord = (vec2<f32>(_e140) / _e143.xy);
    let _e146 = normalizedFragCoord;
    rayNds = ((_e146 * 2f) - vec2(1f));
    let _e150 = rayNds;
    return vec4<f32>(_e150.x, _e150.y, 1f, 1f);
}

fn main_1() {
    var fragCoord_2: vec2<i32>;
    var clipDir: vec4<f32>;
    var param_40: vec2<i32>;
    var viewDir: vec4<f32>;
    var param_41: vec4<f32>;
    var worldDir_1: vec3<f32>;
    var param_42: vec4<f32>;
    var backgroundColor_1: vec4<f32>;
    var param_43: vec3<f32>;
    var sphereCenter_4: vec3<f32>;
    var camDistFromCenter: f32;
    var startPos_3: vec3<f32>;
    var param_44: vec3<f32>;
    var param_45: vec3<f32>;
    var param_46: vec3<f32>;
    var param_47: f32;
    var param_48: vec3<f32>;
    var endPos_1: vec3<f32>;
    var param_49: vec3<f32>;
    var param_50: vec3<f32>;
    var param_51: vec3<f32>;
    var param_52: f32;
    var param_53: vec3<f32>;
    var fogRay: vec3<f32>;
    var hitInner: bool;
    var innerHit: vec3<f32>;
    var param_54: vec3<f32>;
    var param_55: vec3<f32>;
    var param_56: vec3<f32>;
    var param_57: f32;
    var param_58: vec3<f32>;
    var param_59: vec3<f32>;
    var param_60: vec3<f32>;
    var param_61: vec3<f32>;
    var param_62: f32;
    var param_63: vec3<f32>;
    var param_64: vec3<f32>;
    var param_65: vec3<f32>;
    var param_66: vec3<f32>;
    var param_67: f32;
    var param_68: vec3<f32>;
    var param_69: vec3<f32>;
    var param_70: vec3<f32>;
    var param_71: vec3<f32>;
    var param_72: f32;
    var param_73: vec3<f32>;
    var fogAmount: f32;
    var param_74: vec3<f32>;
    var param_75: f32;
    var raymarchResult: RaymarchToCloudResult;
    var param_76: vec3<f32>;
    var param_77: vec3<f32>;
    var param_78: vec3<f32>;
    var sun_1: f32;
    var s: vec3<f32>;
    var bloomColor: vec4<f32>;
    var param_79: vec3<f32>;
    var param_80: f32;
    var cloudAlphaness: f32;
    var alphaness: vec4<f32>;
    var fogAmount_1: f32;
    var param_81: vec3<f32>;
    var param_82: f32;
    var cloud: vec3<f32>;
    var fragColor: vec4<f32>;
    var depth_1: vec4<f32>;
    var param_83: mat4x4<f32>;
    var param_84: mat4x4<f32>;
    var param_85: vec3<f32>;
    var phi_1122_: bool;

    let _e206 = gl_GlobalInvocationID_1;
    fragCoord_2 = bitcast<vec2<i32>>(_e206.xy);
    let _e209 = fragCoord_2;
    param_40 = _e209;
    let _e210 = ComputeClipSpaceDirection_u0028_vi2_u003b((&param_40));
    clipDir = _e210;
    let _e211 = clipDir;
    param_41 = _e211;
    let _e212 = ComputeViewSpaceDirection_u0028_vf4_u003b((&param_41));
    viewDir = _e212;
    let _e213 = viewDir;
    param_42 = _e213;
    let _e214 = ComputeWorldSpaceDirection_u0028_vf4_u003b((&param_42));
    worldDir_1 = _e214;
    let _e215 = worldDir_1;
    param_43 = _e215;
    let _e216 = GetBackgroundColor_u0028_vf3_u003b((&param_43));
    backgroundColor_1 = _e216;
    let _e217 = GetEarthCenter_u0028_();
    sphereCenter_4 = _e217;
    let _e219 = uboCS.cameraPosition;
    let _e221 = sphereCenter_4;
    camDistFromCenter = length((_e219.xyz - _e221));
    let _e224 = camDistFromCenter;
    let _e226 = uboCS.sphereInnerRadius;
    if (_e224 < _e226) {
        let _e229 = uboCS.cameraPosition;
        param_44 = _e229.xyz;
        let _e231 = worldDir_1;
        param_45 = _e231;
        let _e232 = sphereCenter_4;
        param_46 = _e232;
        let _e234 = uboCS.sphereInnerRadius;
        param_47 = _e234;
        let _e235 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_44), (&param_45), (&param_46), (&param_47), (&param_48));
        let _e236 = param_48;
        startPos_3 = _e236;
        let _e238 = uboCS.cameraPosition;
        param_49 = _e238.xyz;
        let _e240 = worldDir_1;
        param_50 = _e240;
        let _e241 = sphereCenter_4;
        param_51 = _e241;
        let _e243 = uboCS.sphereOuterRadius;
        param_52 = _e243;
        let _e244 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_49), (&param_50), (&param_51), (&param_52), (&param_53));
        let _e245 = param_53;
        endPos_1 = _e245;
        let _e246 = startPos_3;
        fogRay = _e246;
    } else {
        let _e247 = camDistFromCenter;
        let _e249 = uboCS.sphereInnerRadius;
        let _e250 = (_e247 >= _e249);
        phi_1122_ = _e250;
        if _e250 {
            let _e251 = camDistFromCenter;
            let _e253 = uboCS.sphereOuterRadius;
            phi_1122_ = (_e251 < _e253);
        }
        let _e256 = phi_1122_;
        if _e256 {
            let _e258 = uboCS.cameraPosition;
            startPos_3 = _e258.xyz;
            let _e261 = uboCS.cameraPosition;
            param_54 = _e261.xyz;
            let _e263 = worldDir_1;
            param_55 = _e263;
            let _e264 = sphereCenter_4;
            param_56 = _e264;
            let _e266 = uboCS.sphereInnerRadius;
            param_57 = _e266;
            let _e267 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_54), (&param_55), (&param_56), (&param_57), (&param_58));
            let _e268 = param_58;
            innerHit = _e268;
            hitInner = _e267;
            let _e269 = hitInner;
            if _e269 {
                let _e270 = innerHit;
                endPos_1 = _e270;
                let _e271 = innerHit;
                fogRay = _e271;
            } else {
                let _e273 = uboCS.cameraPosition;
                param_59 = _e273.xyz;
                let _e275 = worldDir_1;
                param_60 = _e275;
                let _e276 = sphereCenter_4;
                param_61 = _e276;
                let _e278 = uboCS.sphereOuterRadius;
                param_62 = _e278;
                let _e279 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_59), (&param_60), (&param_61), (&param_62), (&param_63));
                let _e280 = param_63;
                endPos_1 = _e280;
                let _e281 = startPos_3;
                fogRay = _e281;
            }
        } else {
            let _e283 = uboCS.cameraPosition;
            param_64 = _e283.xyz;
            let _e285 = worldDir_1;
            param_65 = _e285;
            let _e286 = sphereCenter_4;
            param_66 = _e286;
            let _e288 = uboCS.sphereOuterRadius;
            param_67 = _e288;
            let _e289 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_64), (&param_65), (&param_66), (&param_67), (&param_68));
            let _e290 = param_68;
            startPos_3 = _e290;
            let _e292 = uboCS.cameraPosition;
            param_69 = _e292.xyz;
            let _e294 = worldDir_1;
            param_70 = _e294;
            let _e295 = sphereCenter_4;
            param_71 = _e295;
            let _e297 = uboCS.sphereInnerRadius;
            param_72 = _e297;
            let _e298 = RaySphereIntersection_u0028_vf3_u003b_vf3_u003b_vf3_u003b_f1_u003b_vf3_u003b((&param_69), (&param_70), (&param_71), (&param_72), (&param_73));
            let _e299 = param_73;
            endPos_1 = _e299;
            let _e300 = startPos_3;
            fogRay = _e300;
        }
    }
    let _e301 = fogRay;
    param_74 = _e301;
    param_75 = 0.00006f;
    let _e302 = ComputeFogAmount_u0028_vf3_u003b_f1_u003b((&param_74), (&param_75));
    fogAmount = _e302;
    let _e303 = fogAmount;
    if (_e303 > 0.965f) {
        let _e305 = fragCoord_2;
        let _e306 = backgroundColor_1;
        textureStore(outFragColor, _e305, _e306);
        let _e307 = fragCoord_2;
        let _e308 = backgroundColor_1;
        textureStore(outBloom, _e307, _e308);
        let _e309 = fragCoord_2;
        textureStore(outAlphaness, _e309, vec4<f32>(0f, 0f, 0f, 0f));
        let _e310 = fragCoord_2;
        let _e312 = uboCS.maxDepth;
        textureStore(outCloudDistance, _e310, vec4(_e312));
        return;
    }
    let _e314 = startPos_3;
    param_76 = _e314;
    let _e315 = endPos_1;
    param_77 = _e315;
    let _e316 = backgroundColor_1;
    param_78 = _e316.xyz;
    let _e318 = RaymarchToCloud_u0028_vf3_u003b_vf3_u003b_vf3_u003b((&param_76), (&param_77), (&param_78));
    raymarchResult = _e318;
    let _e320 = raymarchResult.color;
    let _e324 = ((_e320.xyz * 1.8f) - vec3(0.1f));
    raymarchResult.color[0u] = _e324.x;
    raymarchResult.color[1u] = _e324.y;
    raymarchResult.color[2u] = _e324.z;
    let _e335 = raymarchResult.color;
    let _e337 = backgroundColor_1;
    let _e341 = raymarchResult.color[3u];
    let _e343 = fogAmount;
    let _e346 = mix(_e335.xyz, (_e337.xyz * _e341), vec3(clamp(_e343, 0f, 1f)));
    raymarchResult.color[0u] = _e346.x;
    raymarchResult.color[1u] = _e346.y;
    raymarchResult.color[2u] = _e346.z;
    let _e357 = uboCS.lightDirection;
    let _e359 = endPos_1;
    let _e360 = startPos_3;
    sun_1 = clamp(dot(_e357.xyz, normalize((_e359 - _e360))), 0f, 1f);
    let _e365 = sun_1;
    s = (vec3<f32>(0.8f, 0.32f, 0.16f) * pow(_e365, 256f));
    let _e368 = s;
    let _e371 = raymarchResult.color[3u];
    let _e374 = raymarchResult.color;
    let _e376 = (_e374.xyz + (_e368 * _e371));
    raymarchResult.color[0u] = _e376.x;
    raymarchResult.color[1u] = _e376.y;
    raymarchResult.color[2u] = _e376.z;
    let _e386 = backgroundColor_1;
    let _e390 = raymarchResult.color[3u];
    let _e394 = raymarchResult.color;
    let _e396 = ((_e386.xyz * (1f - _e390)) + _e394.xyz);
    backgroundColor_1[0u] = _e396.x;
    backgroundColor_1[1u] = _e396.y;
    backgroundColor_1[2u] = _e396.z;
    backgroundColor_1[3u] = 1f;
    let _e404 = worldDir_1;
    param_79 = _e404;
    param_80 = 128f;
    let _e405 = GetSunColor_u0028_vf3_u003b_f1_u003b((&param_79), (&param_80));
    let _e406 = (_e405 * 1.3f);
    bloomColor = vec4<f32>(_e406.x, _e406.y, _e406.z, 1f);
    let _e413 = raymarchResult.color[3u];
    cloudAlphaness = clamp(_e413, 0.2f, 1f);
    let _e415 = cloudAlphaness;
    alphaness = vec4<f32>(_e415, 0f, 0f, 1f);
    let _e417 = cloudAlphaness;
    if (_e417 > 0.1f) {
        let _e419 = startPos_3;
        param_81 = _e419;
        param_82 = 0.00003f;
        let _e420 = ComputeFogAmount_u0028_vf3_u003b_f1_u003b((&param_81), (&param_82));
        fogAmount_1 = _e420;
        let _e421 = bloomColor;
        let _e423 = fogAmount_1;
        cloud = mix(vec3<f32>(0f, 0f, 0f), _e421.xyz, vec3(clamp(_e423, 0f, 1f)));
        let _e427 = bloomColor;
        let _e429 = cloudAlphaness;
        let _e432 = cloud;
        let _e433 = ((_e427.xyz * (1f - _e429)) + _e432);
        bloomColor[0u] = _e433.x;
        bloomColor[1u] = _e433.y;
        bloomColor[2u] = _e433.z;
    }
    let _e440 = backgroundColor_1;
    let _e441 = _e440.xyz;
    fragColor = vec4<f32>(_e441.x, _e441.y, _e441.z, 1f);
    let _e447 = uboCS.projectionMatrix;
    param_83 = _e447;
    let _e449 = uboCS.viewMatrix;
    param_84 = _e449;
    let _e451 = raymarchResult.position;
    param_85 = _e451;
    let _e452 = ComputeDepth_u0028_mf44_u003b_mf44_u003b_vf3_u003b((&param_83), (&param_84), (&param_85));
    depth_1 = vec4<f32>(_e452, 0f, 0f, 0f);
    let _e454 = fragCoord_2;
    let _e455 = fragColor;
    textureStore(outFragColor, _e454, _e455);
    let _e456 = fragCoord_2;
    let _e457 = bloomColor;
    textureStore(outBloom, _e456, _e457);
    let _e458 = fragCoord_2;
    let _e459 = alphaness;
    textureStore(outAlphaness, _e458, _e459);
    let _e460 = fragCoord_2;
    let _e461 = depth_1;
    textureStore(outCloudDistance, _e460, _e461);
    return;
}

@compute @workgroup_size(16, 16, 1) 
fn main(@builtin(global_invocation_id) gl_GlobalInvocationID: vec3<u32>) {
    gl_GlobalInvocationID_1 = gl_GlobalInvocationID;
    main_1();
}
