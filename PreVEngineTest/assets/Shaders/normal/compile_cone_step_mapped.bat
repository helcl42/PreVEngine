%VULKAN_SDK%/Bin/glslangValidator.exe -V cone_step_mapped.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V cone_step_mapped.frag

move /y frag.spv cone_step_mapped_frag.spv
move /y vert.spv cone_step_mapped_vert.spv

pause
