%VULKAN_SDK%/Bin/glslangValidator.exe -V skybox.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V skybox.frag

move /y frag.spv skybox_frag.spv
move /y vert.spv skybox_vert.spv

pause
