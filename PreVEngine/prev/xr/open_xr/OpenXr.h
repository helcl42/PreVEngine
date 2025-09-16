#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#ifdef ENABLE_XR

#include "common/OpenXrCommon.h"

#include "core/OpenXrCore.h"

#include "input/OpenXrInput.h"
#include "render/OpenXrRender.h"

#include "../IXr.h"

#include <memory>
#include <vector>

namespace prev::xr::open_xr {
class OpenXr final : public IXr {
public:
    OpenXr();

    ~OpenXr();

public:
    std::vector<std::string> GetVulkanInstanceExtensions() const override;

    std::vector<std::string> GetVulkanDeviceExtensions() const override;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const override;

    void CreateSession() override;

    void DestroySession() override;

    void PollEvents() override;

    void PollActions() override;

    bool BeginFrame() override;

    bool EndFrame() override;

    void UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex) override;

    std::vector<VkImage> GetColorImages() const override;

    std::vector<VkImageView> GetColorImagesViews() const override;

    std::vector<VkImage> GetDepthImages() const override;

    std::vector<VkImageView> GetDepthImagesViews() const override;

    bool HasDepthImages() const override;

    VkExtent2D GetExtent() const override;

    VkFormat GetColorFormat() const override;

    VkFormat GetDepthFormat() const override;

    uint32_t GetViewCount() const override;

    uint32_t GetCurrentSwapchainIndex() const override;

    float GetCurrentDeltaTime() const override;

private:
    std::unique_ptr<core::OpenXrCore> m_core{};

    std::unique_ptr<render::OpenXrRender> m_render{};

    std::unique_ptr<input::OpenXrInput> m_input{};
};
} // namespace prev::xr::open_xr

#endif

#endif