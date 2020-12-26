%VULKAN_SDK%/Bin/glslangValidator.exe -V textureless.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V textureless.frag

move /y frag.spv textureless_frag.spv
move /y vert.spv textureless_vert.spv

pause
