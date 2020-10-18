%VULKAN_SDK%/Bin/glslangValidator.exe -V shadow_map_debug.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V shadow_map_debug.frag

move /y frag.spv shadow_map_debug_frag.spv
move /y vert.spv shadow_map_debug_vert.spv

pause
