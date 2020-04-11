%VULKAN_SDK%/Bin/glslangValidator.exe -V font.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -V font.frag

move /y frag.spv font_frag.spv
move /y vert.spv font_vert.spv

pause
