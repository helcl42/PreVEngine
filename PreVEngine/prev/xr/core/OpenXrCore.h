#ifndef __OPEN_XR_CORE_H__
#define __OPEN_XR_CORE_H__

#ifdef ENABLE_XR

#include "OpenXrDebugMessenger.h"

#include "../common/OpenXrCommon.h"
#include "../common/IOpenXrEventObserver.h"

#include "../../common/pattern/Observer.h"

#include <memory>
#include <vector>

namespace prev::xr::core {
class OpenXrCore final {
public:
    OpenXrCore();

    ~OpenXrCore();

public:
    bool RegisterOpenXrEventObserver(common::IOpenXrEventObserver& o);

    bool UnregisterOpenXrEventObserver(common::IOpenXrEventObserver& o);

    std::vector<std::string> GetVulkanInstanceExtensions() const;

    std::vector<std::string> GetVulkanDeviceExtensions() const;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const;

    void CreateSession(const XrGraphicsBindingVulkanKHR& graphicsBinding, const XrViewConfigurationType viewConfiguration);

    void DestroySession();

    void PollEvents();

    bool IsSessionRunning() const;

    XrSession GetSession() const;

    XrSpace GetReferenceSpace() const;

    XrInstance GetInstance() const;

    XrSystemId GetSystemId() const;

    bool IsHandTrackingSupported() const;

private:
    void CreateInstance();

    void DestroyInstance();

    void ShowRuntimeInfo();

    void CreateSystemId();

    void DestroySystemId();

    void CreateDebugMessenger();

    void DestroyDebugMessenger();

    void CreateReferenceSpace();

    void DestroyReferenceSpace();

private:
    XrInstance m_instance{ XR_NULL_HANDLE };
    XrSystemId m_systemId{ XR_NULL_SYSTEM_ID };

    std::vector<const char*> m_activeAPILayers;
    std::vector<const char*> m_activeInstanceExtensions;
    std::vector<std::string> m_apiLayers;
    std::vector<std::string> m_instanceExtensions;

    XrSystemProperties m_systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
    XrSystemHandTrackingPropertiesEXT m_handTrackingSystemProperties{ XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };

    XrGraphicsBindingVulkanKHR m_graphicsBinding{};
    XrViewConfigurationType m_viewConfiguration{ XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM };

    XrSession m_session{ XR_NULL_HANDLE };
    XrSessionState m_sessionState{ XR_SESSION_STATE_UNKNOWN };

    XrSpace m_localSpace{ XR_NULL_HANDLE };

    bool m_applicationRunning{ true };
    bool m_sessionRunning{ false };

    prev::common::pattern::Observer<common::IOpenXrEventObserver> m_eventObserver{};

    std::unique_ptr<OpenXrDebugMessenger> m_debugMessenger{};
};
} // namespace prev::xr::core

#endif

#endif