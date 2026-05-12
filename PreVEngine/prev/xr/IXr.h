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

    virtual GfxAdapter GetPhysicalDevice(GfxInstance instance) const = 0;

    virtual void UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex) = 0;

    virtual void CreateSession() = 0;

    virtual void DestroySession() = 0;

    virtual void PollEvents() = 0;

    virtual void PollActions() = 0;

    virtual bool BeginFrame() = 0;

    virtual bool EndFrame() = 0;

    virtual std::vector<GfxTexture> GetColorImages() const = 0;

    virtual std::vector<GfxTexture> GetDepthImages() const = 0;

    virtual bool HasDepthImages() const = 0;

    virtual GfxExtent2D GetExtent() const = 0;

    virtual GfxFormat GetColorFormat() const = 0;

    virtual GfxFormat GetDepthFormat() const = 0;

    virtual uint32_t GetViewCount() const = 0;

    virtual uint32_t GetCurrentSwapchainIndex() const = 0;

    virtual float GetCurrentDeltaTime() const = 0;
};
} // namespace prev::xr

#endif

#endif