struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

@fragment
fn fragmentMain(@builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S1 : pixelOutput_0 = pixelOutput_0( vec4<f32>(1.0f, 1.0f, 1.0f, 0.0f) );
    return _S1;
}

