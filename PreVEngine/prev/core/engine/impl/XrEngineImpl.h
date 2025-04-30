#ifndef __XR_ENGINE_IMPL_H__
#define __XR_ENGINE_IMPL_H__

#ifdef ENABLE_XR

#include "EngineImpl.h"

#include "../../../xr/OpenXr.h"

namespace prev::core::engine::impl {
class XrEngineImpl final : public EngineImpl {
public:
    XrEngineImpl(const Config& config);

    ~XrEngineImpl();

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

private:
    std::unique_ptr<prev::xr::OpenXr> m_openXr{};
};
} // namespace prev::core::engine::impl

#else 
#error "XR is not enabled. Please enable XR in the build configuration."
#endif

#endif