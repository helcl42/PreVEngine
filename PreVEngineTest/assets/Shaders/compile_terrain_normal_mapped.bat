C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V terrain_normal_mapped.vert
C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V terrain_normal_mapped.frag

move /y frag.spv terrain_normal_mapped_frag.spv
move /y vert.spv terrain_normal_mapped_vert.spv

pause
