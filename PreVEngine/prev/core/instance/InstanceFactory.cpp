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
} // namespace

std::unique_ptr<Instance> InstanceFactory::Create(const std::string& appName, bool enableValidation) const
{
    const GfxBackend backend = GFX_BACKEND_VULKAN;
    if (gfxLoadBackend(backend) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to load Gfx backend");
    }

    if (enableValidation) {
        gfxSetLogCallback(GfxLogHandler, nullptr);
    }

    const char* extensions[] = { GFX_INSTANCE_EXTENSION_SURFACE, GFX_INSTANCE_EXTENSION_DEBUG };
    GfxInstanceDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_INSTANCE_DESCRIPTOR;
    desc.pNext = nullptr;
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