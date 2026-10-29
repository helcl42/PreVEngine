#include "InstanceFactory.h"

#include "../../common/Logger.h"

#include <cstring>
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

    std::vector<const char*> GetSupportedExtensions(GfxBackend backend)
    {
        uint32_t count{ 0 };
        gfxEnumerateInstanceExtensions(backend, &count, nullptr);
        std::vector<const char*> extensions(count);
        gfxEnumerateInstanceExtensions(backend, &count, extensions.data());
        return extensions;
    }

    bool isExtensionSupported(const std::vector<const char*>& supportedExtensions, const char* extension)
    {
        for (const char* supportedExtension : supportedExtensions) {
            if (std::strcmp(supportedExtension, extension) == 0) {
                return true;
            }
        }
        return false;
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

    std::vector<const char*> requestedExtensions{ GFX_INSTANCE_EXTENSION_SURFACE };
    if (enableValidation) {
        requestedExtensions.push_back(GFX_INSTANCE_EXTENSION_DEBUG);
    }
#ifdef ENABLE_XR
    requestedExtensions.push_back(GFX_INSTANCE_EXTENSION_XR_COMPATIBLE);
#endif

    const std::vector<const char*> supportedExtensions{ GetSupportedExtensions(backend) };

    std::vector<const char*> extensions;
    for (const char* extension : requestedExtensions) {
        if (isExtensionSupported(supportedExtensions, extension)) {
            extensions.push_back(extension);
        }
    }

    GfxInstanceDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_INSTANCE_DESCRIPTOR;
    desc.pNext = nativeExtensions.empty() ? nullptr : &nativeExtsDesc;
    desc.backend = backend;
    desc.applicationName = appName.c_str();
    desc.applicationVersion = 1;
    desc.enabledExtensions = extensions.data();
    desc.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

    GfxInstance instance{};
    if (gfxCreateInstance(&desc, &instance) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create gfx instance");
    }

    LOGI("GFX instance created");
    return std::make_unique<Instance>(instance);
}
} // namespace prev::core::instance