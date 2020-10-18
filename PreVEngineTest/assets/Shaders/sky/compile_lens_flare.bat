%VULKAN_SDK%/Bin/glslangValidator.exe -V lens_flare.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V lens_flare.frag

move /y frag.spv lens_flare_frag.spv
move /y vert.spv lens_flare_vert.spv

pause
