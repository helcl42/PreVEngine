%VULKAN_SDK%/Bin/glslangValidator.exe -V default_shadows.vert

move /y vert.spv default_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_shadows.vert

move /y vert.spv animation_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_shadows.vert

move /y vert.spv terrain_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V bump_mapped_shadows.vert

move /y vert.spv bump_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V animation_bump_mapped_shadows.vert

move /y vert.spv animation_bump_mapped_shadows_vert.spv

%VULKAN_SDK%/Bin/glslangValidator.exe -V terrain_bump_mapped_shadows.vert

move /y vert.spv terrain_bump_mapped_shadows_vert.spv

pause
