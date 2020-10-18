%VULKAN_SDK%/Bin/glslangValidator.exe -V particles.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V particles.frag

move /y frag.spv particles_frag.spv
move /y vert.spv particles_vert.spv

pause
