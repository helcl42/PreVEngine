%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_parallax_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_parallax_mapped.frag

move /y frag.spv terrain_parallax_mapped_frag.spv
move /y vert.spv terrain_parallax_mapped_vert.spv

pause
