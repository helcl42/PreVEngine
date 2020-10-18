%VULKAN_SDK%/Bin/glslangValidator.exe -V selection_debug.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V selection_debug.frag

move /y frag.spv selection_debug_frag.spv
move /y vert.spv selection_debug_vert.spv

pause
