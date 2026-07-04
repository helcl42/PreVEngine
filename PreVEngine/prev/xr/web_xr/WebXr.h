#ifndef __WEB_XR_H__
#define __WEB_XR_H__

#ifdef ENABLE_WEBXR

#include "core/WebXrCore.h"

#include "input/WebXrInput.h"
#include "render/WebXrRender.h"

#include "../IXr.h"

#include "../../core/engine/Config.h"

#include <memory>

namespace prev::xr::web_xr {
// Experimental WebXR backend (browser VR/AR over WebGPU, XRGPUBinding). Emscripten-only.
class WebXr final : public IXr {
public:
    explicit WebXr(prev::core::engine::XrMode xrMode);

    ~WebXr() = default;

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

private:
    std::unique_ptr<core::WebXrCore> m_core{};

    std::unique_ptr<render::WebXrRender> m_render{};

    std::unique_ptr<input::WebXrInput> m_input{};
};
} // namespace prev::xr::web_xr

#endif

#endif
