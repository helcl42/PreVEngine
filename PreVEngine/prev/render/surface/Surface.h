#ifndef __RENDER_SURFACE_H__
#define __RENDER_SURFACE_H__

#include "../../core/Core.h"

#include <vector>

namespace prev::render::surface {
class Surface {
public:
    Surface(GfxInstance instance, GfxPlatformWindowHandle windowHandle);

    ~Surface();

public:
    std::vector<GfxFormat> GetSupportedFormats(GfxAdapter adapter) const;

    std::vector<GfxPresentMode> GetSupportedPresentModes(GfxAdapter adapter) const;

    GfxSurfaceInfo GetInfo(GfxAdapter adapter) const;

public:
    operator GfxSurface() const;

private:
    GfxSurface m_surface{};
};
} // namespace prev::render::surface

#endif
