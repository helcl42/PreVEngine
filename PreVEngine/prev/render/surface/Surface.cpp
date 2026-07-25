#include "Surface.h"

#include "../../common/Logger.h"

#include <algorithm>
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
    // The engine renders in gamma space (textures are UNORM, no linearization or tonemap), so shaders
    // output display-ready color. Prefer a plain 8-bit UNORM surface for passthrough: an sRGB surface
    // would re-encode and wash the image out, and some drivers report an HDR float format first (dull).
    // Most-preferred first; fall back to the driver's first choice only if none are supported.
    static constexpr GfxFormat preferredFormats[]{
        GFX_FORMAT_R8G8B8A8_UNORM,
        GFX_FORMAT_B8G8R8A8_UNORM,
    };
    for (const GfxFormat preferred : preferredFormats) {
        if (std::find(formats.begin(), formats.end(), preferred) != formats.end()) {
            LOGI("Preferred surface format found: %d", preferred);
            return preferred;
        }
    }
    LOGW("Preferred surface format not found, using first supported format: %d", formats[0]);
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
