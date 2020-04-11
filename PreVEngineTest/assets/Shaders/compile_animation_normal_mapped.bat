%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_normal_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_normal_mapped.frag

move /y frag.spv animation_normal_mapped_frag.spv
move /y vert.spv animation_normal_mapped_vert.spv

pause
