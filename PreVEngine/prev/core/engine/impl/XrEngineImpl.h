#ifndef __XR_ENGINE_IMPL_H__
#define __XR_ENGINE_IMPL_H__

#ifdef ENABLE_XR

#include "EngineImpl.h"

#include "../../../xr/IXr.h"

namespace prev::core::engine::impl {
class XrEngineImpl final : public EngineImpl {
public:
    XrEngineImpl(const Config& config);

    ~XrEngineImpl();

public:
    void Init() override;

    void ShutDown() override;

    bool Update() override;

    bool BeginFrame() override;

    void PollActions() override;

    bool EndFrame() override;

    void RunFrameLoop(const std::function<bool()>& tick) override;

    uint32_t GetViewCount() const override;

    prev::render::swapchain::ISwapchain& GetSwapchain() const override;

private:
    void ResetInstance() override;

    void ResetDevice() override;

    void ResetRenderPass() override;

    void ResetSwapchain() override;

    void ReleaseSwapchain() override;

    std::unique_ptr<prev::time::ITimeProvider> CreateTimeProvider() const override;

    uint32_t GetPassViewCount() const;

    bool IsDrawingToWindow() const;

    void UpdateSessionState();

private:
    std::unique_ptr<prev::xr::IXr> m_xr{};

    std::unique_ptr<prev::render::swapchain::ISwapchain> m_xrSwapchain{};

    std::unique_ptr<prev::render::swapchain::ISwapchain> m_windowSwapchain{}; // where the session is optional: drawn to while none runs

    bool m_sessionRunning{ false };
};
} // namespace prev::core::engine::impl

#endif

#endif