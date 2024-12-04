#ifndef __XR_ENGINE_IMPL_H__
#define __XR_ENGINE_IMPL_H__

#include "EngineImpl.h"

#include "../../../xr/OpenXR.h"

namespace prev::core::engine::impl {
class XrEngineImpl final : public EngineImpl {
public:
    XrEngineImpl(const Config& config);

    ~XrEngineImpl() = default;

public:
    uint32_t GetViewCount() const override;

    float GetCurrentDeltaTime() const override;

    VkExtent2D GetExtent() const override;

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

private:
    std::shared_ptr<prev::xr::OpenXR> m_openXr{};
};
}

#endif