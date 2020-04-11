%VULKAN_SDK%/Bin/glslangValidator.exe -V raycast_debug.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V raycast_debug.geom
%VULKAN_SDK%/Bin/glslangValidator.exe -V raycast_debug.frag

move /y frag.spv raycast_debug_frag.spv
move /y geom.spv raycast_debug_geom.spv
move /y vert.spv raycast_debug_vert.spv

pause
