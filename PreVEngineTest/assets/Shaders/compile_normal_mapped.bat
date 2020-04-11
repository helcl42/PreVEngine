%VULKAN_SDK%/Bin/glslangValidator.exe -V normal_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V normal_mapped.frag

move /y frag.spv normal_mapped_frag.spv
move /y vert.spv normal_mapped_vert.spv

pause
