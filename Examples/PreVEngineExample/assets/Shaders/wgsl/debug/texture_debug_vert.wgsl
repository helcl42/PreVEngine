struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32, 1>,
    gl_CullDistance: array<f32, 1>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

var<private> unnamed: gl_PerVertex = gl_PerVertex(vec4<f32>(0f, 0f, 0f, 1f), 1f, array<f32, 1>(), array<f32, 1>());
var<private> inPosition_1: vec3<f32>;
var<private> textureCoord: vec2<f32>;
var<private> inTextureCoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;

fn main_1() {
    let _e8 = inPosition_1;
    unnamed.gl_Position = vec4<f32>(_e8.x, _e8.y, _e8.z, 1f);
    let _e14 = inTextureCoord_1;
    textureCoord = _e14;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(1) inTextureCoord: vec2<f32>, @location(2) inNormal: vec3<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTextureCoord_1 = inTextureCoord;
    inNormal_1 = inNormal;
    main_1();
    let _e10 = unnamed.gl_Position.y;
    unnamed.gl_Position.y = -(_e10);
    let _e12 = unnamed.gl_Position;
    let _e13 = textureCoord;
    return VertexOutput(_e12, _e13);
}
