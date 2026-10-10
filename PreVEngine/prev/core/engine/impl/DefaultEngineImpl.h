#ifndef __DEFAULT_ENGINE_IMPL_H__
#define __DEFAULT_ENGINE_IMPL_H__

#include "EngineImpl.h"

namespace prev::core::engine::impl {
class DefaultEngineImpl final : public EngineImpl {
public:
    DefaultEngineImpl(const Config& config);

    ~DefaultEngineImpl();

public:
    void Init() override;

    void ShutDown() override;

    bool Update() override;

    bool BeginFrame() override;

    void PollActions() override;

    bool EndFrame() override;

    prev::render::swapchain::ISwapchain& GetSwapchain() const override;

    uint32_t GetViewCount() const override;

    void RunFrameLoop(const std::function<bool()>& tick) override;

private:
    std::unique_ptr<prev::time::ITimeProvider> CreateTimeProvider() const override;

    void ResetInstance() override;

    void ResetDevice() override;

    void ResetRenderPass() override;

    void ResetSwapchain() override;

    void ReleaseSwapchain() override;

    bool UsesWindowSurface() const override;

private:
    std::unique_ptr<prev::render::swapchain::ISwapchain> m_swapchain{};
};
} // namespace prev::core::engine::impl

#endif