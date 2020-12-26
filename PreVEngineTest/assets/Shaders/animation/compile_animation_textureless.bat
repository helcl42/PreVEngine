%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_textureless.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_textureless.frag

move /y frag.spv animation_textureless_frag.spv
move /y vert.spv animation_textureless_vert.spv

pause
