#ifndef __IXR_H__
#define __IXR_H__

#ifdef ENABLE_XR

#include "../core/Core.h"

#include <functional>
#include <string>
#include <vector>

namespace prev::xr {
// This frame's images. OpenXR: fixed image set acquired by index (imagesChanged == false);
// WebXR: a fresh texture imported every frame (imageIndex == 0, imagesChanged == true).
struct XrFrameImages {
    GfxTexture colorImage{};
    GfxTexture depthImage{}; // null when the backend provides no depth image
    uint32_t imageIndex{};
    bool imagesChanged{}; // the slot's image changed -> rebuild cached views/framebuffers
};

class IXr {
public:
    virtual ~IXr() = default;

public:
    virtual std::vector<std::string> GetRequiredInstanceExtensions() const = 0;

    virtual std::vector<std::string> GetRequiredDeviceExtensions() const = 0;

    virtual GfxAdapter GetAdapter(GfxInstance instance) const = 0;

    virtual void UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, GfxQueue queue) = 0;

    virtual void CreateSession() = 0;

    virtual void DestroySession() = 0;

    virtual void PollEvents() = 0;

    virtual void PollActions() = 0;

    virtual bool BeginFrame() = 0;

    virtual bool EndFrame() = 0;

    virtual bool GetFrameImages(XrFrameImages& outImages) const = 0;

    virtual uint32_t GetImageCount() const = 0;

    virtual bool HasDepthImages() const = 0;

    virtual GfxExtent2D GetExtent() const = 0;

    virtual GfxFormat GetColorFormat() const = 0;

    virtual GfxFormat GetDepthFormat() const = 0;

    virtual uint32_t GetViewCount() const = 0;

    virtual float GetCurrentDeltaTime() const = 0;

    // Invokes tick until it returns false; each backend owns its loop mechanics (WebXR is asynchronous -
    // arms the session's rAF callback and returns immediately).
    virtual void RunFrameLoop(const std::function<bool()>& tick) = 0;
};
} // namespace prev::xr

#endif

#endif
