%VULKAN_SDK%/Bin/glslangValidator.exe -V default.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V default.frag

move /y frag.spv default_frag.spv
move /y vert.spv default_vert.spv

pause
