C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V texture_debug.vert
C:/VulkanSDK/1.2.131.2/Bin/glslangValidator.exe -V texture_debug.frag

move /y frag.spv texture_debug_frag.spv
move /y vert.spv texture_debug_vert.spv

pause
