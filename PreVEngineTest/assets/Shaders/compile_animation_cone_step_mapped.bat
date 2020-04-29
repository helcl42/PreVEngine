%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_cone_step_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_cone_step_mapped.frag

move /y frag.spv animation_cone_step_mapped_frag.spv
move /y vert.spv animation_cone_step_mapped_vert.spv

pause
