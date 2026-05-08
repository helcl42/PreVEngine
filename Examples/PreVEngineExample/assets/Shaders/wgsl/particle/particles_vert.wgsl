struct UniformBufferObject {
    viewMatrices: array<mat4x4<f32>, 1>,
    projectionMatrices: array<mat4x4<f32>, 1>,
    textureNumberOfRows: u32,
}

struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
    @location(1) member_1: vec2<f32>,
    @location(2) member_2: f32,
}

@group(0) @binding(0) 
var<uniform> uboVS: UniformBufferObject;
var<private> rotation_1: f32;
var<private> inPosition_1: vec3<f32>;
var<private> scale_1: vec2<f32>;
var<private> position_1: vec3<f32>;
var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inTextureCoord_1: vec2<f32>;
var<private> outCurrentStageTextureCoord: vec2<f32>;
var<private> textureOffsetsCurrent_1: vec2<f32>;
var<private> outNextStageTextureCoord: vec2<f32>;
var<private> textureOffsetNext_1: vec2<f32>;
var<private> outCurrentNextStageBlendFactor: f32;
var<private> blendFactor_1: f32;
var<private> inNormal_1: vec3<f32>;

fn MakeRotation_u0028_f1_u003b(angle: ptr<function, f32>) -> mat2x2<f32> {
    var s: f32;
    var c: f32;

    let _e28 = (*angle);
    s = sin(_e28);
    let _e30 = (*angle);
    c = cos(_e30);
    let _e32 = c;
    let _e33 = s;
    let _e35 = s;
    let _e36 = c;
    return mat2x2<f32>(vec2<f32>(_e32, -(_e33)), vec2<f32>(_e35, _e36));
}

fn main_1() {
    var viewMatrix: mat4x4<f32>;
    var projectionMatrix: mat4x4<f32>;
    var cameraRightWorldSpace: vec3<f32>;
    var cameraUpWorldSpace: vec3<f32>;
    var localPosition: vec2<f32>;
    var param: f32;
    var vertexPositionWorldspace: vec3<f32>;
    var textureCoordBase: vec2<f32>;

    let _e35 = uboVS.viewMatrices[0i];
    viewMatrix = _e35;
    let _e38 = uboVS.projectionMatrices[0i];
    projectionMatrix = _e38;
    let _e41 = viewMatrix[0][0u];
    let _e44 = viewMatrix[1][0u];
    let _e47 = viewMatrix[2][0u];
    cameraRightWorldSpace = vec3<f32>(_e41, _e44, _e47);
    let _e51 = viewMatrix[0][1u];
    let _e54 = viewMatrix[1][1u];
    let _e57 = viewMatrix[2][1u];
    cameraUpWorldSpace = vec3<f32>(_e51, _e54, _e57);
    let _e59 = rotation_1;
    param = _e59;
    let _e60 = MakeRotation_u0028_f1_u003b((&param));
    let _e62 = inPosition_1[0u];
    let _e64 = scale_1[0u];
    let _e67 = inPosition_1[1u];
    let _e69 = scale_1[1u];
    localPosition = (_e60 * vec2<f32>((_e62 * _e64), (_e67 * _e69)));
    let _e73 = position_1;
    let _e74 = cameraRightWorldSpace;
    let _e76 = localPosition[0u];
    let _e80 = cameraUpWorldSpace;
    let _e82 = localPosition[1u];
    vertexPositionWorldspace = ((_e73 + (_e74 * -(_e76))) + (_e80 * _e82));
    let _e85 = projectionMatrix;
    let _e86 = viewMatrix;
    let _e88 = vertexPositionWorldspace;
    unnamed.gl_Position = ((_e85 * _e86) * vec4<f32>(_e88.x, _e88.y, _e88.z, 1f));
    let _e95 = inTextureCoord_1;
    let _e97 = uboVS.textureNumberOfRows;
    textureCoordBase = (_e95 / vec2(f32(_e97)));
    let _e101 = textureCoordBase;
    let _e102 = textureOffsetsCurrent_1;
    outCurrentStageTextureCoord = (_e101 + _e102);
    let _e104 = textureCoordBase;
    let _e105 = textureOffsetNext_1;
    outNextStageTextureCoord = (_e104 + _e105);
    let _e107 = blendFactor_1;
    outCurrentNextStageBlendFactor = _e107;
    return;
}

@vertex 
fn main(@location(5) rotation: f32, @location(0) inPosition: vec3<f32>, @location(4) scale: vec2<f32>, @location(3) position: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(6) textureOffsetsCurrent: vec2<f32>, @location(7) textureOffsetNext: vec2<f32>, @location(8) blendFactor: f32, @location(2) inNormal: vec3<f32>) -> VertexOutput {
    rotation_1 = rotation;
    inPosition_1 = inPosition;
    scale_1 = scale;
    position_1 = position;
    inTextureCoord_1 = inTextureCoord;
    textureOffsetsCurrent_1 = textureOffsetsCurrent;
    textureOffsetNext_1 = textureOffsetNext;
    blendFactor_1 = blendFactor;
    inNormal_1 = inNormal;
    main_1();
    let _e24 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e24);
    let _e26 = unnamed.gl_Position;
    let _e27 = outCurrentStageTextureCoord;
    let _e28 = outNextStageTextureCoord;
    let _e29 = outCurrentNextStageBlendFactor;
    return VertexOutput(_e26, _e27, _e28, _e29);
}
