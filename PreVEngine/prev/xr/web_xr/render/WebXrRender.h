#ifndef __WEB_XR_RENDER_H__
#define __WEB_XR_RENDER_H__

#ifdef ENABLE_WEBXR

#include "../../IXr.h"

#include <vector>

namespace prev::xr::web_xr::render {
class WebXrRender final {
public:
    WebXrRender();

    ~WebXrRender();

public:
    void UpdateGraphicsBinding(GfxDevice device);

    void OnSessionDestroy();

    bool BeginFrame();

    bool GetFrameImages(XrFrameImages& outImages) const;

    uint32_t GetImageCount() const;

    bool HasDepthImages() const;

    GfxExtent2D GetExtent() const;

    GfxFormat GetColorFormat() const;

    GfxFormat GetDepthFormat() const;

    uint32_t GetViewCount() const;

private:
    void AcquireFrameTextures();

    void ReleaseFrameTextures();

    void PostCameraEvent();

private:
    GfxDevice m_device{};

    std::vector<GfxTexture> m_colorImages; // imported from the XRGPUBinding this frame

    GfxExtent2D m_extent{};

    GfxFormat m_colorFormat{ GFX_FORMAT_B8G8R8A8_UNORM };

    uint32_t m_viewCount{ 2 };
};
} // namespace prev::xr::web_xr::render

#endif

#endif
