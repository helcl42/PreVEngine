%VULKAN_SDK%/Bin/glslangValidator.exe -V bounding_volume_debug.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V bounding_volume_debug.frag

move /y frag.spv bounding_volume_debug_frag.spv
move /y vert.spv bounding_volume_debug_vert.spv

pause
