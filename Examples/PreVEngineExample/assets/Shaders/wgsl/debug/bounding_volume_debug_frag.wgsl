struct BoundingVolumeFSParams_std140_0
{
    @align(16) color_0 : vec4<f32>,
    @align(16) selectedColor_0 : vec4<f32>,
    @align(16) selected_0 : u32,
};

@binding(1) @group(0) var<uniform> uboFS_0 : BoundingVolumeFSParams_std140_0;
struct pixelOutput_0
{
    @location(0) output_0 : vec4<f32>,
};

@fragment
fn fragmentMain(@builtin(position) position_0 : vec4<f32>) -> pixelOutput_0
{
    var _S1 : vec4<f32> = uboFS_0.color_0;
    var resultColor_0 : vec4<f32>;
    if((uboFS_0.selected_0) != u32(0))
    {
        resultColor_0 = uboFS_0.selectedColor_0;
    }
    else
    {
        resultColor_0 = _S1;
    }
    var _S2 : pixelOutput_0 = pixelOutput_0( resultColor_0 );
    return _S2;
}

