#include "ShaderAssetManager.h"

#include "AssetManager.h"

#include <sstream>

namespace prev_test::common {

std::string ShaderAssetManager::GetAssetPath(GfxBackend backend, const std::string& name) const
{
    std::ostringstream path;
    path << "Shaders/" << GetBackendShaderDir(backend) << "/" << name << GetBackendShaderExtension(backend);
    return AssetManager::Instance().GetAssetPath(path.str());
}

const char* ShaderAssetManager::GetBackendShaderDir(GfxBackend backend)
{
    return (backend == GFX_BACKEND_WEBGPU) ? "wgsl" : "spirv";
}

const char* ShaderAssetManager::GetBackendShaderExtension(GfxBackend backend)
{
    return (backend == GFX_BACKEND_WEBGPU) ? ".wgsl" : ".spv";
}

} // namespace prev_test::common
