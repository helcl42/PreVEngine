#ifndef __OPEN_XR_CORE_H__
#define __OPEN_XR_CORE_H__

#ifdef ENABLE_XR

#include "OpenXrDebugMessenger.h"

#include "../common/IOpenXrEventObserver.h"
#include "../common/OpenXrCommon.h"

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

    void CreateReferenceSpace();

    void DestroyReferenceSpace();

private:
    XrInstance m_instance;
    XrSystemId m_systemId;

    XrSystemProperties m_systemProperties;
    XrSystemHandTrackingPropertiesEXT m_handTrackingSystemProperties;

    XrGraphicsBindingVulkanKHR m_graphicsBinding;
    XrViewConfigurationType m_viewConfiguration;

    XrSession m_session;
    XrSessionState m_sessionState;

    XrSpace m_localSpace;

    bool m_applicationRunning;
    bool m_sessionRunning;

    prev::common::pattern::Observer<common::IOpenXrEventObserver> m_eventObserver;

    std::unique_ptr<OpenXrDebugMessenger> m_debugMessenger;
};
} // namespace prev::xr::core

#endif

#endif