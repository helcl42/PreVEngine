#ifndef __WEB_ENGINE_IMPL_H__
#define __WEB_ENGINE_IMPL_H__

#include "EngineImpl.h"

namespace prev::core::engine::impl {
class WebEngineImpl final : public EngineImpl {
public:
    WebEngineImpl(const Config& config);

    ~WebEngineImpl();

public:
    uint32_t GetViewCount() const override;

    float GetCurrentDeltaTime() const override;

    void Init() override;

    void ShutDown() override;

    bool Update() override;

    bool BeginFrame() override;

    void PollActions() override;

    bool EndFrame() override;

private:
    void ResetInstance() override;

    void ResetDevice() override;

    void ResetRenderPass() override;

    void ResetSwapchain() override;
};
} // namespace prev::core::engine::impl

#endif