%VULKAN_SDK%/Bin/glslangValidator.exe -V sun_occlusion.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V sun_occlusion.frag

move /y frag.spv sun_occlusion_frag.spv
move /y vert.spv sun_occlusion_vert.spv

pause
