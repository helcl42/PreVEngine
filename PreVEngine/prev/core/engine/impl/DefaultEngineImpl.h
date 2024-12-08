#ifndef __DEFAULT_ENGINE_IMPL_H__
#define __DEFAULT_ENGINE_IMPL_H__

#include "EngineImpl.h"

namespace prev::core::engine::impl {
class DefaultEngineImpl final : public EngineImpl {
public:
    DefaultEngineImpl(const Config& config);

    ~DefaultEngineImpl() = default;

public:
    uint32_t GetViewCount() const override;

    float GetCurrentDeltaTime() const override;

    void Init() override;

    void ShutDown() override;

    bool Update() override;

    bool BeginFrame() override;

    bool EndFrame() override;

private:
    void ResetInstance() override;

    void ResetDevice() override;

    void ResetRenderPass() override;

    void ResetSwapchain() override;
};
}
#endif