struct SelectionFSParams_std140_0
{
    @align(16) color_0 : vec4<f32>,
};

@binding(1) @group(0) var<uniform> uboFS_0 : SelectionFSParams_std140_0;
struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

@fragment
fn fragmentMain(@builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S1 : pixelOutput_0 = pixelOutput_0( uboFS_0.color_0 );
    return _S1;
}

