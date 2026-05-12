#include "InstanceFactory.h"

#include "../../common/Logger.h"

#include <stdexcept>

namespace prev::core::instance {
namespace {
    void GfxLogHandler(GfxLogLevel level, const char* message, void* /*userData*/)
    {
        switch (level) {
        case GFX_LOG_LEVEL_ERROR:
            LOGE("[gfx] %s", message);
            break;
        case GFX_LOG_LEVEL_WARNING:
            LOGW("[gfx] %s", message);
            break;
        case GFX_LOG_LEVEL_DEBUG:
        case GFX_LOG_LEVEL_INFO:
        default:
            LOGI("[gfx] %s", message);
            break;
        }
    }

    GfxBackend ToGfxBackend(prev::core::engine::RenderBackend renderBackend)
    {
        switch (renderBackend) {
        case prev::core::engine::RenderBackend::WebGPU:
            return GFX_BACKEND_WEBGPU;
        case prev::core::engine::RenderBackend::Vulkan:
        default:
            return GFX_BACKEND_VULKAN;
        }
    }
} // namespace

std::unique_ptr<Instance> InstanceFactory::Create(const std::string& appName, bool enableValidation, prev::core::engine::RenderBackend renderBackend, const std::vector<std::string>& nativeExtensions) const
{
    const GfxBackend backend = ToGfxBackend(renderBackend);
    if (gfxLoadBackend(backend) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to load Gfx backend");
    }

    if (enableValidation) {
        gfxSetLogCallback(GfxLogHandler, nullptr);
    }

    // Build native extensions pNext if needed
    std::vector<const char*> nativeExtPtrs;
    nativeExtPtrs.reserve(nativeExtensions.size());
    for (const auto& ext : nativeExtensions) {
        nativeExtPtrs.push_back(ext.c_str());
    }

    GfxNativeExtensionsDescriptor nativeExtsDesc{};
    nativeExtsDesc.sType = GFX_STRUCTURE_TYPE_NATIVE_EXTENSIONS_DESCRIPTOR;
    nativeExtsDesc.pNext = nullptr;
    nativeExtsDesc.nativeExtensions = nativeExtPtrs.empty() ? nullptr : nativeExtPtrs.data();
    nativeExtsDesc.nativeExtensionCount = static_cast<uint32_t>(nativeExtPtrs.size());

    const char* extensions[] = { GFX_INSTANCE_EXTENSION_SURFACE, GFX_INSTANCE_EXTENSION_DEBUG };
    GfxInstanceDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_INSTANCE_DESCRIPTOR;
    desc.pNext = nativeExtensions.empty() ? nullptr : &nativeExtsDesc;
    desc.backend = backend;
    desc.applicationName = appName.c_str();
    desc.applicationVersion = 1;
    desc.enabledExtensions = extensions;
    desc.enabledExtensionCount = enableValidation ? 2u : 1u;

    GfxInstance instance{};
    if (gfxCreateInstance(&desc, &instance) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create gfx instance");
    }

    LOGI("GFX instance created");
    return std::make_unique<Instance>(instance);
}
} // namespace prev::core::instance