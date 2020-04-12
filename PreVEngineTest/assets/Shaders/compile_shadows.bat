%VULKAN_SDK%/Bin/glslangValidator.exe -V default_shadows.vert

move /y vert.spv default_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_shadows.vert

move /y vert.spv animation_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_shadows.vert

move /y vert.spv terrain_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V normal_mapped_shadows.vert

move /y vert.spv normal_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_normal_mapped_shadows.vert

move /y vert.spv animation_normal_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_normal_mapped_shadows.vert

move /y vert.spv terrain_normal_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V parallax_mapped_shadows.vert

move /y vert.spv parallax_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_parallax_mapped_shadows.vert

move /y vert.spv animation_parallax_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_parallax_mapped_shadows.vert

move /y vert.spv terrain_parallax_mapped_shadows_vert.spv

pause
