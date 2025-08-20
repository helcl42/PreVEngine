#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#ifdef ENABLE_XR

#include "common/OpenXrCommon.h"

#include "core/OpenXrCore.h"

#include "input/OpenXrInput.h"
#include "render/OpenXrRender.h"

#include <memory>
#include <vector>

namespace prev::xr {
class OpenXr final {
public:
    OpenXr();

    ~OpenXr();

public:
    std::vector<std::string> GetVulkanInstanceExtensions() const;

    std::vector<std::string> GetVulkanDeviceExtensions() const;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const;

    void CreateSession();

    void DestroySession();

    void PollEvents();

    void PollActions();

    bool BeginFrame();

    bool EndFrame();

    void UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

    std::vector<VkImage> GetColorImages() const;

    std::vector<VkImageView> GetColorImagesViews() const;

    std::vector<VkImage> GetDepthImages() const;

    std::vector<VkImageView> GetDepthImagesViews() const;

    bool HasDepthImages() const;

    VkExtent2D GetExtent() const;

    VkFormat GetColorFormat() const;

    VkFormat GetDepthFormat() const;

    uint32_t GetViewCount() const;

    uint32_t GetCurrentSwapchainIndex() const;

    float GetCurrentDeltaTime() const;

private:
    std::unique_ptr<core::OpenXrCore> m_core{};

    std::unique_ptr<render::OpenXrRender> m_render{};

    std::unique_ptr<input::OpenXrInput> m_input{};
};
} // namespace prev::xr

#endif

#endif