%VULKAN_SDK%/Bin/glslangValidator.exe -V sky.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V sky.frag

move /y frag.spv sky_frag.spv
move /y vert.spv sky_vert.spv

pause
