C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V shadow_map_debug.vert
C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V shadow_map_debug.frag

move /y frag.spv shadow_map_debug_frag.spv
move /y vert.spv shadow_map_debug_vert.spv

pause
