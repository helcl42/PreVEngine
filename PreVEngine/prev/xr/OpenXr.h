#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#ifdef ENABLE_XR

#include "common/OpenXrCommon.h"
#include "common/IOpenXrEventObserver.h"

#include "render/OpenXrRender.h"
#include "input/OpenXrInput.h"
#include "messenger/OpenXrDebugMessenger.h"

#include "../common/pattern/Observer.h"

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

    VkExtent2D GetExtent() const;

    VkFormat GetColorFormat() const;

    VkFormat GetDepthFormat() const;

    uint32_t GetViewCount() const;

    uint32_t GetCurrentSwapchainIndex() const;

    float GetCurrentDeltaTime() const;

private:
    void CreateInstance();

    void DestroyInstance();

    void GetInstanceProperties();

    void GetSystemId();

    void CreateReferenceSpace();

    void DestroyReferenceSpace();

private:
    std::vector<const char*> m_activeAPILayers;
    std::vector<const char*> m_activeInstanceExtensions;
    std::vector<std::string> m_apiLayers;
    std::vector<std::string> m_instanceExtensions;
    XrInstance m_instance{ XR_NULL_HANDLE };

    XrSystemId m_systemId{ XR_NULL_SYSTEM_ID };
    XrSystemProperties m_systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };

    XrSession m_session{ XR_NULL_HANDLE };
    XrSessionState m_sessionState{ XR_SESSION_STATE_UNKNOWN };

    XrSpace m_localSpace{ XR_NULL_HANDLE };

    bool m_applicationRunning{ true };
    bool m_sessionRunning{ false };

    prev::common::pattern::Observer<common::IOpenXrEventObserver> m_eventObserver;

    std::unique_ptr<messenger::OpenXrDebugMessenger> m_messenger{};

    std::unique_ptr<render::OpenXrRender> m_render{};

    std::unique_ptr<input::OpenXrInput> m_input{};
};
} // namespace prev::xr

#endif

#endif