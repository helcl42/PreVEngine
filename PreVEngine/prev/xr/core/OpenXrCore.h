#ifndef __OPEN_XR_CORE_H__
#define __OPEN_XR_CORE_H__

#ifdef ENABLE_XR

#include "OpenXrDebugMessenger.h"

#include "../common/OpenXrCommon.h"
#include "../common/OpenXrContext.h"
#include "../common/IOpenXrEventObserver.h"

#include "../../common/pattern/Observer.h"

#include <vector>
#include <memory>

namespace prev::xr::core {
class OpenXrCore final {
public:
    explicit OpenXrCore(common::OpenXrContext& context);

    ~OpenXrCore();

public:
    bool RegisterOpenXrEventObserver(common::IOpenXrEventObserver& o);

    bool UnregisterOpenXrEventObserver(common::IOpenXrEventObserver& o);

    void Init();

    void ShutDown();

    std::vector<std::string> GetVulkanInstanceExtensions() const;

    std::vector<std::string> GetVulkanDeviceExtensions() const;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const;

    void CreateSession();

    void DestroySession();

    void PollEvents();

    bool IsSessionRunning() const;

private:
    void CreateInstance();

    void DestroyInstance();

    void GetInstanceProperties();

    void GetSystemId();

    void CreateReferenceSpace();

    void DestroyReferenceSpace();

private:
    common::OpenXrContext& m_context;

    std::vector<const char*> m_activeAPILayers;
    std::vector<const char*> m_activeInstanceExtensions;
    std::vector<std::string> m_apiLayers;
    std::vector<std::string> m_instanceExtensions;

    XrSystemProperties m_systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };

    XrSessionState m_sessionState{ XR_SESSION_STATE_UNKNOWN };

    bool m_applicationRunning{ true };
    bool m_sessionRunning{ false };

    prev::common::pattern::Observer<common::IOpenXrEventObserver> m_eventObserver{};

    std::unique_ptr<OpenXrDebugMessenger> m_debugMessenger{};
};
}

#endif

#endif