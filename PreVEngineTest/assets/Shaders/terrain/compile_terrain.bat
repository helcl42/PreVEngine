%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain.frag

move /y frag.spv terrain_frag.spv
move /y vert.spv terrain_vert.spv

pause
