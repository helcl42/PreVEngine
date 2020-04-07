C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V raycast_debug.vert
C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V raycast_debug.geom
C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V raycast_debug.frag

move /y frag.spv raycast_debug_frag.spv
move /y geom.spv raycast_debug_geom.spv
move /y vert.spv raycast_debug_vert.spv

pause
