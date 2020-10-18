%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_cone_step_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_cone_step_mapped.frag

move /y frag.spv terrain_cone_step_mapped_frag.spv
move /y vert.spv terrain_cone_step_mapped_vert.spv

pause
