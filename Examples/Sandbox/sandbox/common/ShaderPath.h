#ifndef __SANDBOX_SHADER_PATH_H__
#define __SANDBOX_SHADER_PATH_H__

#include <prev/core/Core.h>

#include <string>

#ifdef TARGET_PLATFORM_IOS
#include <IOSUtils.h> // GetResourcesRootPath()
#endif

namespace sandbox::common {
// Resolves a backend-specific shader asset path, e.g. ("sandbox/sandbox_vert") ->
// "Shaders/spirv/sandbox/sandbox_vert.spv" (Vulkan) or ".../wgsl/sandbox/sandbox_vert.wgsl" (WebGPU), then
// prefixes it with the platform's asset root (mirrors the engine's asset-path convention).
inline std::string ShaderPath(GfxBackend backend, const std::string& name)
{
    const bool webgpu{ backend == GFX_BACKEND_WEBGPU };
    const std::string dir{ webgpu ? "wgsl" : "spirv" };
    const std::string ext{ webgpu ? ".wgsl" : ".spv" };
    const std::string relative{ "Shaders/" + dir + "/" + name + ext };

#if defined(TARGET_PLATFORM_ANDROID)
    // Android: assets are read from the APK via the engine's redirected fopen (relative, no prefix).
    return relative;
#elif defined(TARGET_PLATFORM_IOS)
    // iOS: assets are bundle resources.
    return GetResourcesRootPath() + "/" + relative;
#else
    // Desktop + Web: assets are copied/preloaded next to the executable.
    return "./assets/" + relative;
#endif
}
} // namespace sandbox::common

#endif // !__SANDBOX_SHADER_PATH_H__
