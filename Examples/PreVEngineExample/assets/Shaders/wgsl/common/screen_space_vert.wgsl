struct VertexOutput_0
{
    @builtin(position) position_0 : vec4<f32>,
    @location(0) textureCoord_0 : vec2<f32>,
};

struct vertexInput_0
{
    @location(0) position_1 : vec3<f32>,
    @location(1) textureCoord_1 : vec2<f32>,
    @location(2) normal_0 : vec3<f32>,
};

@vertex
fn vertexMain( _S1 : vertexInput_0) -> VertexOutput_0
{
    var output_0 : VertexOutput_0;
    output_0.position_0 = vec4<f32>(_S1.position_1, 1.0f);
    output_0.textureCoord_0 = _S1.textureCoord_1;
    return output_0;
}

