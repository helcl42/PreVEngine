%VULKAN_SDK%/Bin/glslangValidator.exe -V clouds.comp

move /y comp.spv clouds_comp.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V clouds_perlin_worley_noise_3d.comp

move /y comp.spv clouds_perlin_worley_noise_3d_comp.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V sky.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V sky.frag

move /y frag.spv sky_frag.spv
move /y vert.spv sky_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V sky_post_process.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V sky_post_process.frag

move /y frag.spv sky_post_process_frag.spv
move /y vert.spv sky_post_process_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V sky_composite.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V sky_composite.frag

move /y frag.spv sky_composite_frag.spv
move /y vert.spv sky_composite_vert.spv

pause
