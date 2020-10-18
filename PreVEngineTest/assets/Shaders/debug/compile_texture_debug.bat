%VULKAN_SDK%/Bin/glslangValidator.exe -V texture_debug.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V texture_debug.frag

move /y frag.spv texture_debug_frag.spv
move /y vert.spv texture_debug_vert.spv

pause
