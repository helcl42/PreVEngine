#ifndef __IXR_H__
#define __IXR_H__

#ifdef ENABLE_XR

#include "../core/Core.h"

#include <string>
#include <vector>

namespace prev::xr {
class IXr {
public:
    virtual ~IXr() = default;

public:
    virtual std::vector<std::string> GetVulkanInstanceExtensions() const = 0;

    virtual std::vector<std::string> GetVulkanDeviceExtensions() const = 0;

    virtual VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const = 0;

    virtual void CreateSession() = 0;

    virtual void DestroySession() = 0;

    virtual void PollEvents() = 0;

    virtual void PollActions() = 0;

    virtual bool BeginFrame() = 0;

    virtual bool EndFrame() = 0;

    virtual void UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex) = 0;

    virtual std::vector<VkImage> GetColorImages() const = 0;

    virtual std::vector<VkImageView> GetColorImagesViews() const = 0;

    virtual std::vector<VkImage> GetDepthImages() const = 0;

    virtual std::vector<VkImageView> GetDepthImagesViews() const = 0;

    virtual bool HasDepthImages() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual VkFormat GetColorFormat() const = 0;

    virtual VkFormat GetDepthFormat() const = 0;

    virtual uint32_t GetViewCount() const = 0;

    virtual uint32_t GetCurrentSwapchainIndex() const = 0;

    virtual float GetCurrentDeltaTime() const = 0;
};
} // namespace prev::xr

#endif

#endif