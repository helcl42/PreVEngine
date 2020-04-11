%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_normal_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_normal_mapped.frag

move /y frag.spv terrain_normal_mapped_frag.spv
move /y vert.spv terrain_normal_mapped_vert.spv

pause
