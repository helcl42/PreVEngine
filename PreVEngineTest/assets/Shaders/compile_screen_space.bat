%VULKAN_SDK%/Bin/glslangValidator.exe -V screen_space.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V screen_space.frag

move /y frag.spv screen_space_frag.spv
move /y vert.spv screen_space_vert.spv

pause
