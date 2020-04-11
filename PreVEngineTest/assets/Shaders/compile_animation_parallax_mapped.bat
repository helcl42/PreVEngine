%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_parallax_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_parallax_mapped.frag

move /y frag.spv animation_parallax_mapped_frag.spv
move /y vert.spv animation_parallax_mapped_vert.spv

pause
