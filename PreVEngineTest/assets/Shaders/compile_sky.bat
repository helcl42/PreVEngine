%VULKAN_SDK%/Bin/glslangValidator.exe -V sky.comp

move /y comp.spv sky_comp.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V sky_post_process.comp

move /y comp.spv sky_post_process_comp.spv

pause
