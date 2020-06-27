%VULKAN_SDK%/Bin/glslangValidator.exe -V weather.comp

move /y comp.spv weather_comp.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V perlin_worley_noise_3d.comp

move /y comp.spv perlin_worley_noise_3d_comp.spv

pause
