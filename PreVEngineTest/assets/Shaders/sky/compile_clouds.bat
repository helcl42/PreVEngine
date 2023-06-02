%VULKAN_SDK%/Bin/glslangValidator.exe -V clouds.comp

move /y comp.spv clouds_comp.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V clouds_perlin_worley_noise_3d.comp

move /y comp.spv clouds_perlin_worley_noise_3d_comp.spv

pause
