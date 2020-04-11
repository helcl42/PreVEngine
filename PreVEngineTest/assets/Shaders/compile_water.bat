%VULKAN_SDK%/Bin/glslangValidator.exe -V water.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V water.frag

move /y frag.spv water_frag.spv
move /y vert.spv water_vert.spv

pause
