%VULKAN_SDK%/Bin/glslangValidator.exe -V animation.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V animation.frag

move /y frag.spv animation_frag.spv
move /y vert.spv animation_vert.spv

pause
