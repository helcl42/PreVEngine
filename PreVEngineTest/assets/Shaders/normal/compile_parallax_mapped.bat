%VULKAN_SDK%/Bin/glslangValidator.exe -V parallax_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V parallax_mapped.frag

move /y frag.spv parallax_mapped_frag.spv
move /y vert.spv parallax_mapped_vert.spv

pause
