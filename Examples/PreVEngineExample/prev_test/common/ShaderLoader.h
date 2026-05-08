/**
 * @file ShaderLoader.h
 * @brief Backend-aware shader loader that selects SPIR-V or WGSL based on active backend
 */

#pragma once

#include <string>
#include <gfx/gfx.h>

namespace prev_test::common {

class ShaderLoader {
public:
    /**
     * Get shader path for the current backend
     * @param baseName Shader name without extension (e.g., "sky/lens_flare_vert")
     * @param stage Shader stage (VERTEX, FRAGMENT, COMPUTE)
     * @return Full path to appropriate shader file
     * 
     * Example: GetShaderPath("sky/lens_flare_vert", GFX_SHADER_STAGE_VERTEX)
     *   → "assets/Shaders/spirv/sky/lens_flare_vert.spv" (Vulkan)
     *   → "assets/Shaders/wgsl/sky/lens_flare_vert.vert.wgsl" (WebGPU)
     */
    static std::string GetShaderPath(const std::string& baseName, GfxShaderStageFlags stage);
    
    /**
     * Get shader source type for the current backend
     * @return GFX_SHADER_SOURCE_SPIRV for Vulkan, GFX_SHADER_SOURCE_WGSL for WebGPU
     */
    static GfxShaderSourceType GetShaderSourceType();
    
    /**
     * Check if backend supports multiview rendering
     * @return true for Vulkan, false for WebGPU (requires instancing workaround)
     */
    static bool SupportsNativeMultiview();

private:
    static const char* GetBackendShaderDir();
    static const char* GetBackendShaderExtension();
    static const char* GetStageExtension(GfxShaderStageFlags stage);
};

} // namespace prev_test::common
