#ifndef __WEB_XR_H__
#define __WEB_XR_H__

#ifdef ENABLE_WEBXR

#include "../IXr.h"

#include <vector>

namespace prev::xr::web_xr {
// Experimental WebXR backend (browser VR/AR over WebGPU). Emscripten-only (gated in CMake).
//
// Status: session lifecycle (Phase 1) + render path (Phase 2: XRGPUBinding -> Dawn texture import)
// + input polling (Phase 4) are implemented but COMPILE-VERIFIED ONLY - they ride the experimental
// XRGPUBinding API + emdawnwebgpu's JS<->C++ interop and have not been validated on a headset.
// The spots most likely to need on-device adjustment are marked "VERIFY:".
//
// NOTE: the graphics-binding methods (GetRequired*/GetAdapter/UpdateGraphicsBinding) carry no WebXR
// meaning beyond handing over the WebGPU device the browser provides.
class WebXr final : public IXr {
public:
    WebXr();
    ~WebXr();

public:
    std::vector<std::string> GetRequiredInstanceExtensions() const override;

    std::vector<std::string> GetRequiredDeviceExtensions() const override;

    GfxAdapter GetAdapter(GfxInstance instance) const override;

    void UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, GfxQueue queue) override;

    void CreateSession() override;

    void DestroySession() override;

    void PollEvents() override;

    void PollActions() override;

    bool BeginFrame() override;

    bool EndFrame() override;

    bool GetFrameImages(XrFrameImages& outImages) const override;

    uint32_t GetImageCount() const override;

    bool HasDepthImages() const override;

    GfxExtent2D GetExtent() const override;

    GfxFormat GetColorFormat() const override;

    GfxFormat GetDepthFormat() const override;

    uint32_t GetViewCount() const override;

    float GetCurrentDeltaTime() const override;

    void RunFrameLoop(const std::function<bool()>& tick) override;

    // Invoked from the XR session's requestAnimationFrame (via the C trampoline) to render one frame.
    void DispatchFrame();

private:
    // Import this frame's per-view XRGPUBinding color textures into gfx textures (called from BeginFrame).
    void AcquireFrameTextures();

    // Release the gfx wrappers around the previous frame's imported textures.
    void ReleaseFrameTextures();

    // Post the per-eye poses + FOVs (derived from the XR view projection matrices) to the engine cameras.
    void PostCameraEvent();

private:
    std::function<bool()> m_frameCallback{}; // engine tick; invoked from the XR session's rAF, false => stop the loop
    GfxDevice m_device{}; // WebGPU device the session renders with (from UpdateGraphicsBinding)
    std::vector<GfxTexture> m_colorImages; // per-view textures imported from the binding this frame
    GfxExtent2D m_extent{};
    GfxFormat m_colorFormat{ GFX_FORMAT_B8G8R8A8_UNORM };
    uint32_t m_viewCount{ 2 };
};
} // namespace prev::xr::web_xr

#endif

#endif
