#include "ShaderLoader.h"

#include "../render/renderer/sky/SkyEvents.h"
#include <sstream>

namespace prev_test::common {

std::string ShaderLoader::GetShaderPath(const std::string& baseName, GfxShaderStageFlags stage)
{
    std::ostringstream path;
    path << "Shaders/" << GetBackendShaderDir() << "/" << baseName << GetStageExtension(stage) << GetBackendShaderExtension();
    return path.str();
}

GfxShaderSourceType ShaderLoader::GetShaderSourceType()
{
#ifdef GFX_BACKEND_VULKAN
    return GFX_SHADER_SOURCE_SPIRV;
#elif defined(GFX_BACKEND_WEBGPU)
    return GFX_SHADER_SOURCE_WGSL;
#else
    return GFX_SHADER_SOURCE_SPIRV;
#endif
}

bool ShaderLoader::SupportsNativeMultiview()
{
#ifdef GFX_BACKEND_VULKAN
    return true;
#elif defined(GFX_BACKEND_WEBGPU)
    return false;  // WebGPU requires instancing workaround
#else
    return false;
#endif
}

const char* ShaderLoader::GetBackendShaderDir()
{
#ifdef GFX_BACKEND_VULKAN
    return "spirv";
#elif defined(GFX_BACKEND_WEBGPU)
    return "wgsl";
#else
    return "spirv";
#endif
}

const char* ShaderLoader::GetBackendShaderExtension()
{
#ifdef GFX_BACKEND_VULKAN
    return ".spv";
#elif defined(GFX_BACKEND_WEBGPU)
    return ".wgsl";
#else
    return ".spv";
#endif
}

const char* ShaderLoader::GetStageExtension(GfxShaderStageFlags stage)
{
    switch (stage) {
    case GFX_SHADER_STAGE_VERTEX:
        return ".vert";
    case GFX_SHADER_STAGE_FRAGMENT:
        return ".frag";
    case GFX_SHADER_STAGE_COMPUTE:
        return ".comp";
    default:
        return "";
    }
}

} // namespace prev_test::common
