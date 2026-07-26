#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#ifdef ENABLE_OPENXR

#include "common/OpenXrCommon.h"

#include "core/OpenXrCore.h"

#include "input/OpenXrInput.h"
#include "render/OpenXrRender.h"

#include "../IXr.h"

#include "../../core/engine/Config.h"

#include <memory>
#include <vector>

namespace prev::xr::open_xr {
class OpenXr final : public IXr {
public:
    OpenXr(prev::core::engine::XrMode xrMode, bool colorManaged);

    ~OpenXr();

public:
    std::vector<std::string> GetRequiredInstanceExtensions() const override;

    std::vector<std::string> GetRequiredDeviceExtensions() const override;

    GfxAdapter GetAdapter(GfxInstance instance) const override;

    void CreateSession() override;

    void DestroySession() override;

    void PollEvents() override;

    void PollActions() override;

    bool BeginFrame() override;

    bool EndFrame() override;

    void UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, GfxQueue queue) override;

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
    bool m_loaded{ false };

    std::unique_ptr<core::OpenXrCore> m_core{};

    std::unique_ptr<render::OpenXrRender> m_render{};

    std::unique_ptr<input::OpenXrInput> m_input{};
};
} // namespace prev::xr::open_xr

#endif

#endif