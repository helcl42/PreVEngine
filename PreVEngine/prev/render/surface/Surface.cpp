#include "Surface.h"

#include "../../common/Logger.h"

#include <stdexcept>

namespace prev::render::surface {
Surface::Surface(GfxInstance instance, GfxPlatformWindowHandle windowHandle)
{
    GfxSurfaceDescriptor surfaceDesc{};
    surfaceDesc.sType = GFX_STRUCTURE_TYPE_SURFACE_DESCRIPTOR;
    surfaceDesc.pNext = nullptr;
    surfaceDesc.label = "Main Surface";
    surfaceDesc.windowHandle = windowHandle;
    if (gfxInstanceCreateSurface(instance, &surfaceDesc, &m_surface) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create gfx surface");
    }
    LOGI("Surface created");
}

Surface::~Surface()
{
    if (m_surface) {
        gfxSurfaceDestroy(m_surface);
        m_surface = nullptr;
        LOGI("Surface destroyed");
    }
}

Surface::operator GfxSurface() const
{
    return m_surface;
}

std::vector<GfxFormat> Surface::GetSupportedFormats(GfxAdapter adapter) const
{
    uint32_t count{ 0 };
    gfxSurfaceEnumerateSupportedFormats(m_surface, adapter, &count, nullptr);
    if (count == 0) {
        return {};
    }
    std::vector<GfxFormat> formats(count);
    gfxSurfaceEnumerateSupportedFormats(m_surface, adapter, &count, formats.data());
    return formats;
}

GfxFormat Surface::GetPreferredFormat(GfxAdapter adapter) const
{
    const auto formats = GetSupportedFormats(adapter);
    if (formats.empty()) {
        throw std::runtime_error("No supported surface formats found for the GPU");
    }
    return formats[0];
}

std::vector<GfxPresentMode> Surface::GetSupportedPresentModes(GfxAdapter adapter) const
{
    uint32_t count{ 0 };
    gfxSurfaceEnumerateSupportedPresentModes(m_surface, adapter, &count, nullptr);
    if (count == 0) {
        return {};
    }
    std::vector<GfxPresentMode> presentModes(count);
    gfxSurfaceEnumerateSupportedPresentModes(m_surface, adapter, &count, presentModes.data());
    return presentModes;
}

GfxPresentMode Surface::GetPreferredPresentMode(GfxAdapter adapter, GfxPresentMode preferred) const
{
    const auto modes = GetSupportedPresentModes(adapter);
    for (const auto& m : modes) {
        if (m == preferred) {
            return preferred;
        }
    }
    return modes.empty() ? GFX_PRESENT_MODE_FIFO : modes[0];
}

GfxSurfaceInfo Surface::GetInfo(GfxAdapter adapter) const
{
    GfxSurfaceInfo info{};
    gfxSurfaceGetInfo(m_surface, adapter, &info);
    return info;
}
} // namespace prev::render::surface
