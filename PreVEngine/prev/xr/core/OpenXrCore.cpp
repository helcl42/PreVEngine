#include "OpenXrCore.h"

#ifdef ENABLE_XR

#include "../XrEvents.h"
#include "../util/OpenXRUtil.h"

#include "../../event/EventChannel.h"

#include <sstream>

namespace prev::xr::core {
namespace {
    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR{};
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR{};
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR{};

    inline void LoadXrExtensionFunctions(XrInstance xrInstance)
    {
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR),
            "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR),
            "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR),
            "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR),
            "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");
    }

    inline std::vector<XrApiLayerProperties> GetAllApiLayers()
    {
        uint32_t apiLayerCount{ 0 };
        std::vector<XrApiLayerProperties> apiLayerProperties;
        OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
        apiLayerProperties.resize(apiLayerCount, prev::xr::util::CreateStruct<XrApiLayerProperties>(XR_TYPE_API_LAYER_PROPERTIES));
        OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");
        return apiLayerProperties;
    }

    inline std::vector<XrExtensionProperties> GetAllInstanceExtensions()
    {
        uint32_t extensionCount{ 0 };
        std::vector<XrExtensionProperties> extensionProperties;
        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
        extensionProperties.resize(extensionCount, prev::xr::util::CreateStruct<XrExtensionProperties>(XR_TYPE_EXTENSION_PROPERTIES));
        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");
        return extensionProperties;
    }

    inline bool ContainsApiLayer(const std::vector<XrApiLayerProperties>& items, const char* name)
    {
        for (const auto& item : items) {
            if (strcmp(name, item.layerName) == 0) {
                return true;
            }
        }
        return false;
    }

    inline bool ContainsInstanceExtension(const std::vector<XrExtensionProperties>& items, const char* name)
    {
        for (const auto& item : items) {
            if (strcmp(name, item.extensionName) == 0) {
                return true;
            }
        }
        return false;
    }

    inline bool ContainsString(const std::vector<const char*>& list, const char* name)
    {
        for (const auto& item : list) {
            if (strcmp(name, item) == 0) {
                return true;
            }
        }
        return false;
    }

    inline std::vector<const char*> GetActiveInstanceExtension(const std::vector<XrExtensionProperties>& availableInstanceExtension, const std::vector<const char*>& requestedItemNames)
    {
        std::vector<const char*> activeInstanceExtensions;
        for (const auto& requestedItemName : requestedItemNames) {
            if (ContainsInstanceExtension(availableInstanceExtension, requestedItemName)) {
                activeInstanceExtensions.push_back(requestedItemName);
            } else {
                LOGE("Failed to find OpenXR Instance extension: %s", requestedItemName);
            }
        }
        return activeInstanceExtensions;
    }

    inline std::vector<const char*> GetActiveApiLayers(const std::vector<XrApiLayerProperties>& availableApiLayers, const std::vector<const char*>& requestedApiLayerNames)
    {
        std::vector<const char*> activeApiLayers;
        for (auto& requestedApiLayerName : requestedApiLayerNames) {
            if (ContainsApiLayer(availableApiLayers, requestedApiLayerName)) {
                activeApiLayers.push_back(requestedApiLayerName);
            } else {
                LOGE("Failed to find OpenXR API layer: %s", requestedApiLayerName);
            }
        }
        return activeApiLayers;
    }
} // namespace

OpenXrCore::OpenXrCore()
    : m_instance{ XR_NULL_HANDLE }
    , m_systemId{ XR_NULL_SYSTEM_ID }
    , m_systemProperties{ prev::xr::util::CreateStruct<XrSystemProperties>(XR_TYPE_SYSTEM_PROPERTIES) }
    , m_handTrackingSystemProperties{ prev::xr::util::CreateStruct<XrSystemHandTrackingPropertiesEXT>(XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT) }
    , m_graphicsBinding{}
    , m_viewConfiguration{ XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM }
    , m_session{ XR_NULL_HANDLE }
    , m_sessionState{ XR_SESSION_STATE_UNKNOWN }
    , m_localSpace{ XR_NULL_HANDLE }
    , m_applicationRunning{ true }
    , m_sessionRunning{ false }
    , m_eventObserver{}
    , m_debugMessenger{}
{
    CreateInstance();
    CreateSystemId();

    ShowRuntimeInfo();
}

OpenXrCore::~OpenXrCore()
{
    DestroySystemId();
    DestroyInstance();
}

bool OpenXrCore::RegisterOpenXrEventObserver(common::IOpenXrEventObserver& o)
{
    return m_eventObserver.Register(o);
}

bool OpenXrCore::UnregisterOpenXrEventObserver(common::IOpenXrEventObserver& o)
{
    return m_eventObserver.Unregister(o);
}

std::vector<std::string> OpenXrCore::GetVulkanInstanceExtensions() const
{
    uint32_t extensionNamesSize{};
    OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_instance, m_systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

    std::vector<char> extensionNames(extensionNamesSize);
    OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_instance, m_systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

    std::stringstream streamData(extensionNames.data());
    std::vector<std::string> extensions;
    std::string extension;
    while (std::getline(streamData, extension, ' ')) {
        extensions.push_back(extension);
    }
    return extensions;
}

std::vector<std::string> OpenXrCore::GetVulkanDeviceExtensions() const
{
    uint32_t extensionNamesSize{};
    OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_instance, m_systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Device Extensions.");

    std::vector<char> extensionNames(extensionNamesSize);
    OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_instance, m_systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Device Extensions.");

    std::stringstream streamData(extensionNames.data());
    std::vector<std::string> extensions;
    std::string extension;
    while (std::getline(streamData, extension, ' ')) {
        extensions.push_back(extension);
    }
    return extensions;
}

VkPhysicalDevice OpenXrCore::GetPhysicalDevice(VkInstance instance) const
{
    XrGraphicsRequirementsVulkanKHR graphicsRequirements{ prev::xr::util::CreateStruct<XrGraphicsRequirementsVulkanKHR>(XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR) };
    OPENXR_CHECK(xrGetVulkanGraphicsRequirementsKHR(m_instance, m_systemId, &graphicsRequirements), "Failed to get Graphics Requirements for Vulkan.");

    VkPhysicalDevice physicalDeviceFromXR;
    OPENXR_CHECK(xrGetVulkanGraphicsDeviceKHR(m_instance, m_systemId, instance, &physicalDeviceFromXR), "Failed to get Graphics Device for Vulkan.");
    return physicalDeviceFromXR;
}

void OpenXrCore::CreateSession(const XrGraphicsBindingVulkanKHR& graphicsBinding, const XrViewConfigurationType viewConfiguration)
{
    m_graphicsBinding = graphicsBinding;
    m_viewConfiguration = viewConfiguration;

    XrSessionCreateInfo sessionCreateInfo{ prev::xr::util::CreateStruct<XrSessionCreateInfo>(XR_TYPE_SESSION_CREATE_INFO) };
    sessionCreateInfo.next = &m_graphicsBinding;
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = m_systemId;

    OPENXR_CHECK(xrCreateSession(m_instance, &sessionCreateInfo, &m_session), "Failed to create Session.");

    CreateReferenceSpace();
}

void OpenXrCore::DestroySession()
{
    DestroyReferenceSpace();

    OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
}

void OpenXrCore::PollEvents()
{
    auto XrPollEvents = [](XrInstance instance, XrEventDataBuffer& outEvent) -> bool {
        outEvent = { XR_TYPE_EVENT_DATA_BUFFER };
        return xrPollEvent(instance, &outEvent) == XR_SUCCESS;
    };

    XrEventDataBuffer eventData{ prev::xr::util::CreateStruct<XrEventDataBuffer>(XR_TYPE_EVENT_DATA_BUFFER) };
    while (XrPollEvents(m_instance, eventData)) {
        switch (eventData.type) {
        case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
            XrEventDataEventsLost* eventsLost = reinterpret_cast<XrEventDataEventsLost*>(&eventData);
            LOGI("OPENXR: Events Lost: %d", eventsLost->lostEventCount);
            break;
        }
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
            XrEventDataInstanceLossPending* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData);
            LOGI("OPENXR: Instance Loss Pending at: %lld", instanceLossPending->lossTime);
            m_sessionRunning = false;
            m_applicationRunning = false;
            break;
        }
        case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
            XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData);
            LOGI("OPENXR: Interaction Profile changed for Session: %p", static_cast<void*>(interactionProfileChanged->session));
            if (interactionProfileChanged->session != m_session) {
                LOGW("XrEventDataInteractionProfileChanged for unknown Session");
                break;
            }
            break;
        }
        case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
            XrEventDataReferenceSpaceChangePending* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData);
            LOGI("OPENXR: Reference Space Change pending for Session: %p", static_cast<void*>(referenceSpaceChangePending->session));
            if (referenceSpaceChangePending->session != m_session) {
                LOGW("XrEventDataReferenceSpaceChangePending for unknown Session");
                break;
            }
            break;
        }
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
            XrEventDataSessionStateChanged* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
            if (sessionStateChanged->session != m_session) {
                LOGW("XrEventDataSessionStateChanged for unknown Session");
                break;
            }

            if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
                XrSessionBeginInfo sessionBeginInfo{ prev::xr::util::CreateStruct<XrSessionBeginInfo>(XR_TYPE_SESSION_BEGIN_INFO) };
                sessionBeginInfo.primaryViewConfigurationType = m_viewConfiguration;
                OPENXR_CHECK(xrBeginSession(m_session, &sessionBeginInfo), "Failed to begin Session.");
                m_sessionRunning = true;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
                OPENXR_CHECK(xrEndSession(m_session), "Failed to end Session.");
                m_sessionRunning = false;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
                m_sessionRunning = false;
                m_applicationRunning = false;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_LOSS_PENDING) {
                // SessionState is loss pending. Exit the application.
                // It's possible to try a reestablish an XrInstance and XrSession, but we will simply exit here.
                m_sessionRunning = false;
                m_applicationRunning = false;
            }
            // Store state for reference across the application.
            m_sessionState = sessionStateChanged->state;
            break;
        }
        default: {
            break;
        }
        }

        for (auto& o : m_eventObserver.GetObservers()) {
            o->OnEvent(eventData);
        }
    }
}

bool OpenXrCore::IsSessionRunning() const
{
    return m_sessionRunning;
}

XrSession OpenXrCore::GetSession() const
{
    return m_session;
}

XrSpace OpenXrCore::GetReferenceSpace() const
{
    return m_localSpace;
}

XrInstance OpenXrCore::GetInstance() const
{
    return m_instance;
}

XrSystemId OpenXrCore::GetSystemId() const
{
    return m_systemId;
}

bool OpenXrCore::IsHandTrackingSupported() const
{
    return m_handTrackingSystemProperties.supportsHandTracking;
}

void OpenXrCore::CreateInstance()
{
    const std::vector<const char*> requestedApiLayerNames = {
        // no API layers
    };

    const std::vector<const char*> requestedInstanceExtensionNames = {
 #if defined(TARGET_PLATFORM_ANDROID) && defined(ENABLE_XR_EXTENDED_INIT)
         XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME,
 #endif
        XR_EXT_DEBUG_UTILS_EXTENSION_NAME,

        XR_KHR_VULKAN_ENABLE_EXTENSION_NAME,
        XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME,

        XR_EXT_HAND_TRACKING_EXTENSION_NAME,
        XR_EXT_HAND_INTERACTION_EXTENSION_NAME,
    };

    const auto allApiLayers{ GetAllApiLayers() };
    const auto activeApiLayers{ GetActiveApiLayers(allApiLayers, requestedApiLayerNames) };

    const auto allInstanceExtensions{ GetAllInstanceExtensions() };
    const auto activeInstanceExtensions{ GetActiveInstanceExtension(allInstanceExtensions, requestedInstanceExtensionNames) };

    XrApplicationInfo applicationInfo{};
    strncpy(applicationInfo.applicationName, "OpenXR PreVEngineTest", XR_MAX_APPLICATION_NAME_SIZE);
    applicationInfo.applicationVersion = 1;
    strncpy(applicationInfo.engineName, "OpenXR PreVEngine", XR_MAX_ENGINE_NAME_SIZE);
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    XrInstanceCreateInfo instanceCreateInfo{ prev::xr::util::CreateStruct<XrInstanceCreateInfo>(XR_TYPE_INSTANCE_CREATE_INFO) };
    instanceCreateInfo.createFlags = 0;
    instanceCreateInfo.applicationInfo = applicationInfo;
    instanceCreateInfo.enabledApiLayerCount = static_cast<uint32_t>(activeApiLayers.size());
    instanceCreateInfo.enabledApiLayerNames = activeApiLayers.data();
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
    instanceCreateInfo.enabledExtensionNames = activeInstanceExtensions.data();

#if defined(TARGET_PLATFORM_ANDROID) && defined(ENABLE_XR_EXTENDED_INIT)
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid{ prev::xr::util::CreateStruct<XrInstanceCreateInfoAndroidKHR>(XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR) };
    instanceCreateInfoAndroid.applicationActivity = g_AndroidApp->activity;
    instanceCreateInfoAndroid.applicationVM = g_AndroidApp->activity->vm;
    instanceCreateInfo.next = &instanceCreateInfoAndroid;
#endif

    OPENXR_CHECK(xrCreateInstance(&instanceCreateInfo, &m_instance), "Failed to create Instance.");
    if (!m_instance) {
        throw std::runtime_error("Could not create XR instance.");
    }

    if (ContainsString(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_debugMessenger = std::make_unique<OpenXrDebugMessenger>(m_instance);
    }

    LoadXrExtensionFunctions(m_instance);
}

void OpenXrCore::DestroyInstance()
{
    m_debugMessenger = {};

    OPENXR_CHECK(xrDestroyInstance(m_instance), "Failed to destroy Instance.");
}

void OpenXrCore::ShowRuntimeInfo()
{
    XrInstanceProperties instanceProperties{ prev::xr::util::CreateStruct<XrInstanceProperties>(XR_TYPE_INSTANCE_PROPERTIES) };
    OPENXR_CHECK(xrGetInstanceProperties(m_instance, &instanceProperties), "Failed to get InstanceProperties.");

    LOGI("OpenXR Runtime: %s - %d.%d.%d", instanceProperties.runtimeName, XR_VERSION_MAJOR(instanceProperties.runtimeVersion), XR_VERSION_MINOR(instanceProperties.runtimeVersion), XR_VERSION_PATCH(instanceProperties.runtimeVersion));
}

void OpenXrCore::CreateSystemId()
{
    XrSystemGetInfo systemGetInfo{ prev::xr::util::CreateStruct<XrSystemGetInfo>(XR_TYPE_SYSTEM_GET_INFO) };
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    OPENXR_CHECK(xrGetSystem(m_instance, &systemGetInfo, &m_systemId), "Failed to get SystemID.");

    m_systemProperties.next = &m_handTrackingSystemProperties;

    OPENXR_CHECK(xrGetSystemProperties(m_instance, m_systemId, &m_systemProperties), "Failed to get SystemProperties.");
}

void OpenXrCore::DestroySystemId()
{
    m_systemId = XR_NULL_SYSTEM_ID;
}

void OpenXrCore::CreateReferenceSpace()
{
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{ prev::xr::util::CreateStruct<XrReferenceSpaceCreateInfo>(XR_TYPE_REFERENCE_SPACE_CREATE_INFO) };
    referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    referenceSpaceCreateInfo.poseInReferenceSpace = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
    OPENXR_CHECK(xrCreateReferenceSpace(m_session, &referenceSpaceCreateInfo, &m_localSpace), "Failed to create ReferenceSpace.");
}

void OpenXrCore::DestroyReferenceSpace()
{
    OPENXR_CHECK(xrDestroySpace(m_localSpace), "Failed to destroy Space.")
}
} // namespace prev::xr::core

#endif