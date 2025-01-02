#include "OpenXr.h"

#ifdef ENABLE_XR

#include "XrEvents.h"

#include "../event/EventChannel.h"
#include "../util/VkUtils.h"

#include <sstream>

namespace prev::xr {
namespace {
    PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT{};
    PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT{};

    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT{};
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT{};
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT{};

    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR{};
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR{};
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR{};

    PFN_xrCreateFoveationProfileFB pfnCreateFoveationProfileFB{};
    PFN_xrDestroyFoveationProfileFB pfnDestroyFoveationProfileFB{};
    PFN_xrUpdateSwapchainFB pfnUpdateSwapchainFB{};

    inline bool IsStringInVector(const std::vector<const char*>& list, const char* name)
    {
        bool found{ false };
        for (const auto& item : list) {
            if (strcmp(name, item) == 0) {
                found = true;
                break;
            }
        }
        return found;
    }

    template <typename T>
    inline bool BitwiseCheck(const T& value, const T& checkValue)
    {
        return ((value & checkValue) == checkValue);
    }

    XrBool32
    OpenXrMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        // Lambda to covert an XrDebugUtilsMessageSeverityFlagsEXT to std::string. Bitwise check to concatenate multiple severities to the output string.
        auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity) -> std::string {
            bool separator = false;

            std::string msgFlags;
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
                msgFlags += "VERBOSE";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "INFO";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "WARN";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "ERROR";
            }
            return msgFlags;
        };
        // Lambda to covert an XrDebugUtilsMessageTypeFlagsEXT to std::string. Bitwise check to concatenate multiple types to the output string.
        auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType) -> std::string {
            bool separator = false;

            std::string msgFlags;
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
                msgFlags += "GEN";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "SPEC";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "PERF";
            }
            return msgFlags;
        };

        // Collect message data.
        std::string functionName = (pCallbackData->functionName) ? pCallbackData->functionName : "";
        std::string messageSeverityStr = GetMessageSeverityString(messageSeverity);
        std::string messageTypeStr = GetMessageTypeString(messageType);
        std::string messageId = (pCallbackData->messageId) ? pCallbackData->messageId : "";
        std::string message = (pCallbackData->message) ? pCallbackData->message : "";

        // String stream final message.
        std::stringstream errorMessage;
        errorMessage << functionName << "(" << messageSeverityStr << " / " << messageTypeStr << "): msgNum: " << messageId << " - " << message;

        // Log and debug break.
        LOGE("%s", errorMessage.str().c_str());
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
            ASSERT(false, "Error...");
        }
        return XrBool32();
    }

    XrDebugUtilsMessengerEXT CreateOpenXrDebugUtilsMessenger(const XrInstance xrInstance)
    {
        // Fill out a XrDebugUtilsMessengerCreateInfoEXT structure specifying all severities and types.
        // Set the userCallback to OpenXRMessageCallbackFunction().
        XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{ XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
        debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXrMessageCallbackFunction;
        debugUtilsMessengerCI.userData = nullptr;

        // Load xrCreateDebugUtilsMessengerEXT() function pointer as it is not default loaded by the OpenXR loader.
        XrDebugUtilsMessengerEXT debugUtilsMessenger{};

        // Finally create and return the XrDebugUtilsMessengerEXT.
        OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(xrInstance, &debugUtilsMessengerCI, &debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
        return debugUtilsMessenger;
    }

    void DestroyOpenXrDebugUtilsMessenger(const XrInstance xrInstance, XrDebugUtilsMessengerEXT debugUtilsMessenger)
    {
        OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
    }

    std::vector<std::string> GetInstanceExtensionsForOpenXr(const XrInstance instance, const XrSystemId systemId)
    {
        uint32_t extensionNamesSize = 0;
        OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(instance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

        std::vector<char> extensionNames(extensionNamesSize);
        OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(instance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

        std::stringstream streamData(extensionNames.data());
        std::vector<std::string> extensions;
        std::string extension;
        while (std::getline(streamData, extension, ' ')) {
            extensions.push_back(extension);
        }
        return extensions;
    }

    std::vector<std::string> GetDeviceExtensionsForOpenXr(const XrInstance instance, const XrSystemId systemId)
    {
        uint32_t extensionNamesSize = 0;
        OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(instance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Device Extensions.");

        std::vector<char> extensionNames(extensionNamesSize);
        OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(instance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Device Extensions.");

        std::stringstream streamData(extensionNames.data());
        std::vector<std::string> extensions;
        std::string extension;
        while (std::getline(streamData, extension, ' ')) {
            extensions.push_back(extension);
        }
        return extensions;
    }

    XrPath ConvertStringToXrPath(const XrInstance xrInstance, const char* pathString)
    {
        XrPath xrPath;
        OPENXR_CHECK(xrStringToPath(xrInstance, pathString, &xrPath), "Failed to create XrPath from string.");
        return xrPath;
    }

    std::string ConvertXrPathToString(const XrInstance xrInstance, const XrPath& path)
    {
        uint32_t strl;
        char text[XR_MAX_PATH_LENGTH];
        XrResult res;
        res = xrPathToString(xrInstance, path, XR_MAX_PATH_LENGTH, &strl, text);
        std::string str;
        if (res == XR_SUCCESS) {
            str = text;
        } else {
            OPENXR_CHECK(res, "Failed to retrieve path.");
        }
        return str;
    }

    void LoadXrExtensionFunctions(const XrInstance xrInstance)
    {
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrCreateDebugUtilsMessengerEXT), "Failed to get xrCreateDebugUtilsMessengerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrDestroyDebugUtilsMessengerEXT), "Failed to get xrDestroyDebugUtilsMessengerEXT.");

        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)&xrCreateHandTrackerEXT), "Failed to get xrCreateHandTrackerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)&xrDestroyHandTrackerEXT), "Failed to get xrDestroyHandTrackerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)&xrLocateHandJointsEXT), "Failed to get xrLocateHandJointsEXT.");

        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");

        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrCreateFoveationProfileFB", (PFN_xrVoidFunction*)(&pfnCreateFoveationProfileFB)), "Could not find function xrCreateFoveationProfileFB");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrDestroyFoveationProfileFB", (PFN_xrVoidFunction*)(&pfnDestroyFoveationProfileFB)), "Could not find function xrDestroyFoveationProfileFB");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrUpdateSwapchainFB", (PFN_xrVoidFunction*)(&pfnUpdateSwapchainFB)), "Could not find function xrUpdateSwapchainFB");
    }
} // namespace

OpenXr::OpenXr()
{
    CreateInstance();
    CreateDebugMessenger();
    GetInstanceProperties();
    GetSystemID();

    CreateActionSet();
    SuggestBindings();

    GetViewConfigurationViews();
    GetEnvironmentBlendModes();
}

OpenXr::~OpenXr()
{
    DestroyDebugMessenger();
    DestroyInstance();
}

std::vector<std::string> OpenXr::GetVulkanInstanceExtensions() const
{
    return GetInstanceExtensionsForOpenXr(m_xrInstance, m_systemID);
}

std::vector<std::string> OpenXr::GetVulkanDeviceExtensions() const
{
    return GetDeviceExtensionsForOpenXr(m_xrInstance, m_systemID);
}

VkPhysicalDevice OpenXr::GetPhysicalDevice(VkInstance instance) const
{
    XrGraphicsRequirementsVulkanKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
    OPENXR_CHECK(xrGetVulkanGraphicsRequirementsKHR(m_xrInstance, m_systemID, &graphicsRequirements), "Failed to get Graphics Requirements for Vulkan.");

    VkPhysicalDevice physicalDeviceFromXR;
    OPENXR_CHECK(xrGetVulkanGraphicsDeviceKHR(m_xrInstance, m_systemID, instance, &physicalDeviceFromXR), "Failed to get Graphics Device for Vulkan.");
    return physicalDeviceFromXR;
}

void OpenXr::InitializeGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    m_graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
    m_graphicsBinding.instance = instance;
    m_graphicsBinding.physicalDevice = physicalDevice;
    m_graphicsBinding.device = device;
    m_graphicsBinding.queueFamilyIndex = queueFamilyIndex;
    m_graphicsBinding.queueIndex = queueIndex;
}

void OpenXr::DestroyGraphicsBinding()
{
    m_graphicsBinding = {};
}

void OpenXr::CreateReferenceSpace()
{
    // Fill out an XrReferenceSpaceCreateInfo structure and create a reference XrSpace, specifying a Local space with an identity pose as the origin.
    XrReferenceSpaceCreateInfo referenceSpaceCI{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    referenceSpaceCI.poseInReferenceSpace = { { 0.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f } };
    OPENXR_CHECK(xrCreateReferenceSpace(m_session, &referenceSpaceCI, &m_localSpace), "Failed to create ReferenceSpace.");
}

void OpenXr::DestroyReferenceSpace()
{
    // Destroy the reference XrSpace.
    OPENXR_CHECK(xrDestroySpace(m_localSpace), "Failed to destroy Space.")
}

void OpenXr::CreateSwapchains()
{
    // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
    uint32_t formatCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &formatCount, nullptr), "Failed to enumerate Swapchain Formats");
    std::vector<int64_t> formats(formatCount);
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, formatCount, &formatCount, formats.data()), "Failed to enumerate Swapchain Formats");

    auto colorFormatIter = std::find(formats.begin(), formats.end(), m_preferredColorFormat);
    if (colorFormatIter == formats.cend()) {
        LOGE("Failed to find color format for Swapchain.");
    }

    auto depthFormatIter = std::find(formats.begin(), formats.end(), m_preferredDepthFormat);
    if (depthFormatIter == formats.cend()) {
        LOGE("Failed to find depth format for Swapchain.");
    }

    bool coherentViews = m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    for (const XrViewConfigurationView& viewConfigurationView : m_viewConfigurationViews) {
        // Check the current view size against the first view.
        coherentViews |= m_viewConfigurationViews[0].recommendedImageRectWidth == viewConfigurationView.recommendedImageRectWidth;
        coherentViews |= m_viewConfigurationViews[0].recommendedImageRectHeight == viewConfigurationView.recommendedImageRectHeight;
    }
    if (!coherentViews) {
        LOGE("The views are not coherent. Unable to create a single Swapchain.");
    }

    const XrViewConfigurationView& viewConfigurationView = m_viewConfigurationViews[0];
    const uint32_t viewCount = static_cast<uint32_t>(m_viewConfigurationViews.size());

    // Color
    XrSwapchainCreateInfo colorSwapchainCI{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
    colorSwapchainCI.createFlags = 0;
    colorSwapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
    colorSwapchainCI.format = m_preferredColorFormat;
    colorSwapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount; // Use the recommended values from the XrViewConfigurationView.
    colorSwapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
    colorSwapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
    colorSwapchainCI.faceCount = 1;
    colorSwapchainCI.arraySize = viewCount;
    colorSwapchainCI.mipCount = 1;

    OPENXR_CHECK(xrCreateSwapchain(m_session, &colorSwapchainCI, &m_colorSwapchainInfo.swapchain), "Failed to create Color Swapchain");
    m_colorSwapchainInfo.swapchainFormat = static_cast<VkFormat>(colorSwapchainCI.format); // Save the swapchain format for later use.

    // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
    uint32_t colorSwapchainImageCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, 0, &colorSwapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");

    swapchainImagesMap[m_colorSwapchainInfo.swapchain].first = SwapchainType::COLOR;
    swapchainImagesMap[m_colorSwapchainInfo.swapchain].second.resize(colorSwapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
    XrSwapchainImageBaseHeader* colorSwapchainImages = reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImagesMap[m_colorSwapchainInfo.swapchain].second.data());

    OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, colorSwapchainImageCount, &colorSwapchainImageCount, colorSwapchainImages), "Failed to enumerate Color Swapchain Images.");

    for (uint32_t j = 0; j < colorSwapchainImageCount; ++j) {
        const auto image{ swapchainImagesMap[m_colorSwapchainInfo.swapchain].second[j].image };
        const auto imageView{ prev::util::vk::CreateImageView(m_graphicsBinding.device, image, m_preferredColorFormat, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1, VK_IMAGE_ASPECT_COLOR_BIT, viewCount, 0) };
        m_colorSwapchainInfo.images.push_back(image);
        m_colorSwapchainInfo.imageViews.push_back(imageView);
    }

    // Depth.
    XrSwapchainCreateInfo depthSwapchainCI{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
    depthSwapchainCI.createFlags = 0;
    depthSwapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthSwapchainCI.format = m_preferredDepthFormat;
    depthSwapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount; // Use the recommended values from the XrViewConfigurationView.
    depthSwapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
    depthSwapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
    depthSwapchainCI.faceCount = 1;
    depthSwapchainCI.arraySize = viewCount;
    depthSwapchainCI.mipCount = 1;
    OPENXR_CHECK(xrCreateSwapchain(m_session, &depthSwapchainCI, &m_depthSwapchainInfo.swapchain), "Failed to create Depth Swapchain");
    m_depthSwapchainInfo.swapchainFormat = static_cast<VkFormat>(depthSwapchainCI.format); // Save the swapchain format for later use.

    uint32_t depthSwapchainImageCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, 0, &depthSwapchainImageCount, nullptr), "Failed to enumerate Depth Swapchain Images.");

    swapchainImagesMap[m_depthSwapchainInfo.swapchain].first = SwapchainType::DEPTH;
    swapchainImagesMap[m_depthSwapchainInfo.swapchain].second.resize(depthSwapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
    XrSwapchainImageBaseHeader* depthSwapchainImages = reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImagesMap[m_depthSwapchainInfo.swapchain].second.data());

    OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, depthSwapchainImageCount, &depthSwapchainImageCount, depthSwapchainImages), "Failed to enumerate Depth Swapchain Images.");

    for (uint32_t j = 0; j < depthSwapchainImageCount; ++j) {
        const auto image{ swapchainImagesMap[m_depthSwapchainInfo.swapchain].second[j].image };
        const auto imageView{ prev::util::vk::CreateImageView(m_graphicsBinding.device, image, m_preferredDepthFormat, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1, VK_IMAGE_ASPECT_DEPTH_BIT, viewCount, 0) };
        m_depthSwapchainInfo.images.push_back(image);
        m_depthSwapchainInfo.imageViews.push_back(imageView);
    }
}

void OpenXr::DestroySwapchains()
{
    // Destroy the color and depth image views from GraphicsAPI.
    for (auto& imageView : m_colorSwapchainInfo.imageViews) {
        vkDestroyImageView(m_graphicsBinding.device, imageView, VK_NULL_HANDLE);
    }
    for (auto& imageView : m_depthSwapchainInfo.imageViews) {
        vkDestroyImageView(m_graphicsBinding.device, imageView, VK_NULL_HANDLE);
    }

    // Free the Swapchain Image Data.
    swapchainImagesMap[m_colorSwapchainInfo.swapchain].second.clear();
    swapchainImagesMap.erase(m_colorSwapchainInfo.swapchain);
    swapchainImagesMap[m_depthSwapchainInfo.swapchain].second.clear();
    swapchainImagesMap.erase(m_depthSwapchainInfo.swapchain);

    // Destroy the swapchains.
    OPENXR_CHECK(xrDestroySwapchain(m_colorSwapchainInfo.swapchain), "Failed to destroy Color Swapchain");
    OPENXR_CHECK(xrDestroySwapchain(m_depthSwapchainInfo.swapchain), "Failed to destroy Depth Swapchain");
}

void OpenXr::CreateSession()
{
    // Create an XrSessionCreateInfo structure.
    XrSessionCreateInfo sessionCI{ XR_TYPE_SESSION_CREATE_INFO };
    sessionCI.next = &m_graphicsBinding;
    sessionCI.createFlags = 0;
    sessionCI.systemId = m_systemID;

    OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCI, &m_session), "Failed to create Session.");

    CreateActionPoses();
    AttachActionSet();

    CreateReferenceSpace();
    CreateSwapchains();
}

void OpenXr::DestroySession()
{
    DestroySwapchains();
    DestroyReferenceSpace();

    // Destroy the XrSession.
    OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
}

std::vector<VkImage> OpenXr::GetColorImages() const
{
    return m_colorSwapchainInfo.images;
}

std::vector<VkImageView> OpenXr::GetColorImagesViews() const
{
    return m_colorSwapchainInfo.imageViews;
}

std::vector<VkImage> OpenXr::GetDepthImages() const
{
    return m_depthSwapchainInfo.images;
}

std::vector<VkImageView> OpenXr::GetDepthImagesViews() const
{
    return m_depthSwapchainInfo.imageViews;
}

VkExtent2D OpenXr::GetExtent() const
{
    return { m_viewConfigurationViews[0].recommendedImageRectWidth, m_viewConfigurationViews[0].recommendedImageRectHeight };
}

VkFormat OpenXr::GetColorFormat() const
{
    return m_preferredColorFormat;
}

VkFormat OpenXr::GetDepthFormat() const
{
    return m_preferredDepthFormat;
}

uint32_t OpenXr::GetViewCount() const
{
    return static_cast<uint32_t>(m_viewConfigurationViews.size());
}

uint32_t OpenXr::GetCurrentSwapchainIndex() const
{
    return m_currentSwapchainIndex;
}

float OpenXr::GetCurrentDeltaTime() const
{
    return m_currentDeltaTime;
}

bool OpenXr::BeginFrame()
{
    if (!m_sessionRunning) {
        return false;
    }

    // Get the XrFrameState for timing and rendering info.
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
    OPENXR_CHECK(xrWaitFrame(m_session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

    // Tell the OpenXR compositor that the application is beginning the frame.
    XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
    OPENXR_CHECK(xrBeginFrame(m_session, &frameBeginInfo), "Failed to begin the XR Frame.");

    if (m_frameState.predictedDisplayTime == 0) {
        m_currentDeltaTime = 0.0;
    } else {
        m_currentDeltaTime = static_cast<float>(frameState.predictedDisplayTime - m_frameState.predictedDisplayTime) * 1e-9f;
    }
    m_frameState = frameState;

    // Locate the views from the view configuration within the (reference) space at the display time.
    std::vector<XrView> views(m_viewConfigurationViews.size(), { XR_TYPE_VIEW });

    XrViewState viewState{ XR_TYPE_VIEW_STATE }; // Will contain information on whether the position and/or orientation is valid and/or tracked.
    XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
    viewLocateInfo.viewConfigurationType = m_viewConfiguration;
    viewLocateInfo.displayTime = frameState.predictedDisplayTime;
    viewLocateInfo.space = m_localSpace;
    uint32_t viewCount = 0;
    XrResult result = xrLocateViews(m_session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
    if (result != XR_SUCCESS) {
        LOGE("Failed to locate Views.");
        return false;
    }

    XrCameraEvent event{};
    for (size_t i = 0; i < views.size(); ++i) {
        const auto& view{ views[i] };
        event.poses[i] = prev::util::math::Pose{ { view.pose.orientation.w, view.pose.orientation.x, view.pose.orientation.y, view.pose.orientation.z }, { view.pose.position.x, view.pose.position.y, view.pose.position.z } };
        event.fovs[i] = prev::util::math::Fov{ view.fov.angleLeft, view.fov.angleRight, view.fov.angleUp, view.fov.angleDown };
    }
    event.count = static_cast<uint32_t>(views.size());
    prev::event::EventChannel::Post(event);

    m_renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;
    m_renderLayerInfo.layerProjectionViews.resize(viewCount, { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW });
    m_renderLayerInfo.layerDepthInfos.resize(viewCount, { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR });

    // Acquire and wait for an image from the swapchains.
    // Get the image index of an image in the swapchains.
    // The timeout is infinite.
    uint32_t colorImageIndex = 0;
    uint32_t depthImageIndex = 0;
    XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
    OPENXR_CHECK(xrAcquireSwapchainImage(m_colorSwapchainInfo.swapchain, &acquireInfo, &colorImageIndex), "Failed to acquire Image from the Color Swapchian");
    OPENXR_CHECK(xrAcquireSwapchainImage(m_depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");

    XrSwapchainImageWaitInfo waitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
    waitInfo.timeout = XR_INFINITE_DURATION;
    OPENXR_CHECK(xrWaitSwapchainImage(m_colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
    OPENXR_CHECK(xrWaitSwapchainImage(m_depthSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Depth Swapchain");

    // Get the width and height and construct the viewport and scissors.
    const uint32_t& width = m_viewConfigurationViews[0].recommendedImageRectWidth;
    const uint32_t& height = m_viewConfigurationViews[0].recommendedImageRectHeight;

    // Fill out the XrCompositionLayerProjectionView structure specifying the pose and fov from the view.
    // This also associates the swapchain image with this layer projection view.
    // Per view in the view configuration:
    for (uint32_t i = 0; i < viewCount; ++i) {
        // projection color layer
        m_renderLayerInfo.layerProjectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
        m_renderLayerInfo.layerProjectionViews[i].pose = views[i].pose;
        m_renderLayerInfo.layerProjectionViews[i].fov = views[i].fov;
        m_renderLayerInfo.layerProjectionViews[i].subImage.swapchain = m_colorSwapchainInfo.swapchain;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.x = 0;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.y = 0;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width = static_cast<int32_t>(width);
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height = static_cast<int32_t>(height);
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex = i; // Useful for multiview rendering.

        // depth layer
        m_renderLayerInfo.layerProjectionViews[i].next = &m_renderLayerInfo.layerDepthInfos[i];

        m_renderLayerInfo.layerDepthInfos[i] = { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR };
        m_renderLayerInfo.layerDepthInfos[i].subImage.swapchain = m_depthSwapchainInfo.swapchain;
        m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.x = 0;
        m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.y = 0;
        m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.width = static_cast<int32_t>(width);
        m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.height = static_cast<int32_t>(height);
        m_renderLayerInfo.layerDepthInfos[i].minDepth = m_minDepth;
        m_renderLayerInfo.layerDepthInfos[i].maxDepth = m_maxDepth;
        m_renderLayerInfo.layerDepthInfos[i].nearZ = m_nearClippingPlane;
        m_renderLayerInfo.layerDepthInfos[i].farZ = m_farClippingPlane;
    }

    m_currentSwapchainIndex = colorImageIndex;
    return true;
}

bool OpenXr::EndFrame()
{
    if (!m_sessionRunning) {
        return false;
    }

    // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
    XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
    OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
    OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");

    m_renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    m_renderLayerInfo.layerProjection.space = m_localSpace;
    m_renderLayerInfo.layerProjection.viewCount = static_cast<uint32_t>(m_renderLayerInfo.layerProjectionViews.size());
    m_renderLayerInfo.layerProjection.views = m_renderLayerInfo.layerProjectionViews.data();

    m_renderLayerInfo.layers.resize(1);
    m_renderLayerInfo.layers[0] = reinterpret_cast<XrCompositionLayerBaseHeader*>(&m_renderLayerInfo.layerProjection);

    // Tell OpenXR that we are finished with this frame; specifying its display time, environment blending and layers.
    XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
    frameEndInfo.displayTime = m_frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = m_environmentBlendMode;
    frameEndInfo.layerCount = static_cast<uint32_t>(m_renderLayerInfo.layers.size());
    frameEndInfo.layers = m_renderLayerInfo.layers.data();
    OPENXR_CHECK(xrEndFrame(m_session, &frameEndInfo), "Failed to end the XR Frame.");
    return true;
}

void OpenXr::operator()(const XrCameraFeedbackEvent& event)
{
    m_nearClippingPlane = event.nearClippingPlane;
    m_farClippingPlane = event.fatClippingPlane;
    m_minDepth = event.minDepth;
    m_maxDepth = event.maxDepth;
}

void OpenXr::CreateInstance()
{
    XrApplicationInfo AI;
    strncpy(AI.applicationName, "OpenXR PreVEngineTest", XR_MAX_APPLICATION_NAME_SIZE);
    AI.applicationVersion = 1;
    strncpy(AI.engineName, "OpenXR PreVEngine", XR_MAX_ENGINE_NAME_SIZE);
    AI.engineVersion = 1;
    AI.apiVersion = XR_CURRENT_API_VERSION;

    {
        // XR_DOCS_TAG_BEGIN_instanceExtensions
        m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
        // Ensure m_apiType is already defined when we call this line.
        m_instanceExtensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
        // XR_DOCS_TAG_END_instanceExtensions
        // XR_DOCS_TAG_BEGIN_handTrackingExtensions
        m_instanceExtensions.push_back(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
        m_instanceExtensions.push_back(XR_EXT_HAND_INTERACTION_EXTENSION_NAME);
        // XR_DOCS_TAG_END_handTrackingExtensions
        // XR_DOCS_TAG_BEGIN_CompositionLayerDepthExtensions
        m_instanceExtensions.push_back(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
        // XR_DOCS_TAG_END_CompositionLayerDepthExtensions
    }

    // Get all the API Layers from the OpenXR runtime.
    uint32_t apiLayerCount = 0;
    std::vector<XrApiLayerProperties> apiLayerProperties;
    OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
    apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
    OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

    // Check the requested API layers against the ones from the OpenXR. If found add it to the Active API Layers.
    for (auto& requestLayer : m_apiLayers) {
        for (auto& layerProperty : apiLayerProperties) {
            // strcmp returns 0 if the strings match.
            if (strcmp(requestLayer.c_str(), layerProperty.layerName) != 0) {
                continue;
            } else {
                m_activeAPILayers.push_back(requestLayer.c_str());
                break;
            }
        }
    }

    // Get all the Instance Extensions from the OpenXR instance.
    uint32_t extensionCount = 0;
    std::vector<XrExtensionProperties> extensionProperties;
    OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
    extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
    OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");

    // Check the requested Instance Extensions against the ones from the OpenXR runtime.
    // If an extension is found add it to Active Instance Extensions.
    // Log error if the Instance Extension is not found.
    for (auto& requestedInstanceExtension : m_instanceExtensions) {
        bool found = false;
        for (auto& extensionProperty : extensionProperties) {
            // strcmp returns 0 if the strings match.
            if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
                continue;
            } else {
                m_activeInstanceExtensions.push_back(requestedInstanceExtension.c_str());
                found = true;
                break;
            }
        }
        if (!found) {
            LOGE("Failed to find OpenXR instance extension: %s", requestedInstanceExtension.c_str());
        }
    }

    // Fill out an XrInstanceCreateInfo structure and create an XrInstance.
    XrInstanceCreateInfo instanceCI{ XR_TYPE_INSTANCE_CREATE_INFO };
    instanceCI.createFlags = 0;
    instanceCI.applicationInfo = AI;
    instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_activeAPILayers.size());
    instanceCI.enabledApiLayerNames = m_activeAPILayers.data();
    instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_activeInstanceExtensions.size());
    instanceCI.enabledExtensionNames = m_activeInstanceExtensions.data();
    OPENXR_CHECK(xrCreateInstance(&instanceCI, &m_xrInstance), "Failed to create Instance.");

    if (!m_xrInstance) {
        throw std::runtime_error("Could not create XR instance.");
    }

    LoadXrExtensionFunctions(m_xrInstance);
}

void OpenXr::DestroyInstance()
{
    // Destroy the XrInstance.
    OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
}

void OpenXr::CreateDebugMessenger()
{
    // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
    if (IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_debugUtilsMessenger = CreateOpenXrDebugUtilsMessenger(m_xrInstance); // From OpenXRDebugUtils.h.
    }
}

void OpenXr::DestroyDebugMessenger()
{
    // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
    if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
        DestroyOpenXrDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger); // From OpenXRDebugUtils.h.
    }
}

void OpenXr::GetInstanceProperties()
{
    // Get the instance's properties and log the runtime name and version.
    XrInstanceProperties instanceProperties{ XR_TYPE_INSTANCE_PROPERTIES };
    OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

    LOGI("OpenXR Runtime: %s - %d.%d.%d", instanceProperties.runtimeName, XR_VERSION_MAJOR(instanceProperties.runtimeVersion), XR_VERSION_MINOR(instanceProperties.runtimeVersion), XR_VERSION_PATCH(instanceProperties.runtimeVersion));
}

void OpenXr::GetSystemID()
{
    // Get the XrSystemId from the instance and the supplied XrFormFactor.
    XrSystemGetInfo systemGI{ XR_TYPE_SYSTEM_GET_INFO };
    systemGI.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_systemID), "Failed to get SystemID.");

    // Get the System's properties for some general information about the hardware and the vendor.
    OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");
}

void OpenXr::GetViewConfigurationViews()
{
    // Gets the View Configuration Types. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, 0, &viewConfigurationCount, nullptr), "Failed to enumerate View Configurations.");
    m_viewConfigurations.resize(viewConfigurationCount);
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, viewConfigurationCount, &viewConfigurationCount, m_viewConfigurations.data()), "Failed to enumerate View Configurations.");

    // Pick the first application supported View Configuration Type con supported by the hardware.
    for (const XrViewConfigurationType& viewConfiguration : m_applicationViewConfigurations) {
        if (std::find(m_viewConfigurations.begin(), m_viewConfigurations.end(), viewConfiguration) != m_viewConfigurations.end()) {
            m_viewConfiguration = viewConfiguration;
            break;
        }
    }
    if (m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
        LOGE("Failed to find a view configuration type. Defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO.");
        m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    }

    // Gets the View Configuration Views. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationViewCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, 0, &viewConfigurationViewCount, nullptr), "Failed to enumerate ViewConfiguration Views.");
    m_viewConfigurationViews.resize(viewConfigurationViewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");

    if (viewConfigurationCount > MAX_VIEW_COUNT) {
        LOGE("OpenXR view configuration count > maxViewCount: %d > %d", viewConfigurationCount, MAX_VIEW_COUNT);
    }
}

void OpenXr::GetEnvironmentBlendModes()
{
    // Retrieves the available blend modes. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t environmentBlendModeCount = 0;
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, 0, &environmentBlendModeCount, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
    m_environmentBlendModes.resize(environmentBlendModeCount);
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, environmentBlendModeCount, &environmentBlendModeCount, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

    // Pick the first application supported blend mode supported by the hardware.
    for (const XrEnvironmentBlendMode& environmentBlendMode : m_applicationEnvironmentBlendModes) {
        if (std::find(m_environmentBlendModes.begin(), m_environmentBlendModes.end(), environmentBlendMode) != m_environmentBlendModes.end()) {
            m_environmentBlendMode = environmentBlendMode;
            break;
        }
    }
    if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
        LOGE("Failed to find a compatible blend mode. Defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE.");
        m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    }
}

void OpenXr::CreateActionSet()
{
    XrActionSetCreateInfo actionSetCI{ XR_TYPE_ACTION_SET_CREATE_INFO };
    actionSetCI.priority = 0;
    strncpy(actionSetCI.actionSetName, "pre-v-engine-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
    strncpy(actionSetCI.localizedActionSetName, "pre-v-engine-action-set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
    OPENXR_CHECK(xrCreateActionSet(m_xrInstance, &actionSetCI, &m_actionSet), "Failed to create ActionSet.");

    auto CreateAction = [this](const char* name, const XrActionType xrActionType, const std::vector<const char*>& subactionPaths = {}) -> XrAction {
        XrActionCreateInfo actionCI{ XR_TYPE_ACTION_CREATE_INFO };
        actionCI.actionType = xrActionType;
        std::vector<XrPath> subactionXrPaths;
        for (const auto& p : subactionPaths) {
            subactionXrPaths.push_back(ConvertStringToXrPath(m_xrInstance, p));
        }
        actionCI.countSubactionPaths = static_cast<uint32_t>(subactionXrPaths.size());
        actionCI.subactionPaths = subactionXrPaths.data();
        // The internal name the runtime uses for this Action.
        strncpy(actionCI.actionName, name, XR_MAX_ACTION_NAME_SIZE);
        // Localized names are required so there is a human-readable action name to show the user if they are rebinding the Action in an options screen.
        strncpy(actionCI.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);

        XrAction xrAction{};
        OPENXR_CHECK(xrCreateAction(m_actionSet, &actionCI, &xrAction), "Failed to create Action.");
        return xrAction;
    };

    // TODO - read this from a file - make a general action mechanism?
    m_squeezeAction = CreateAction("squeeze", XR_ACTION_TYPE_FLOAT_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_triggerAction = CreateAction("trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_palmPoseAction = CreateAction("pose", XR_ACTION_TYPE_POSE_INPUT, { "/user/hand/left", "/user/hand/right" });

    m_handPaths[0] = ConvertStringToXrPath(m_xrInstance, "/user/hand/left");
    m_handPaths[1] = ConvertStringToXrPath(m_xrInstance, "/user/hand/right");
}

void OpenXr::DestroyActionSet()
{
    // nothing to do here ??
}

void OpenXr::AttachActionSet()
{
    // Attach the action set we just made to the session. We could attach multiple action sets!
    XrSessionActionSetsAttachInfo actionSetAttachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
    actionSetAttachInfo.countActionSets = 1;
    actionSetAttachInfo.actionSets = &m_actionSet;
    OPENXR_CHECK(xrAttachSessionActionSets(m_session, &actionSetAttachInfo), "Failed to attach ActionSet to Session.");
}

void OpenXr::DetachActionSet()
{
    // nothing to do here ??
}

void OpenXr::PollEvents()
{
    // Poll OpenXR for a new event.
    XrEventDataBuffer eventData{ XR_TYPE_EVENT_DATA_BUFFER };
    auto XrPollEvents = [&]() -> bool {
        eventData = { XR_TYPE_EVENT_DATA_BUFFER };
        return xrPollEvent(m_xrInstance, &eventData) == XR_SUCCESS;
    };

    while (XrPollEvents()) {
        switch (eventData.type) {
        // Log the number of lost events from the runtime.
        case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
            XrEventDataEventsLost* eventsLost = reinterpret_cast<XrEventDataEventsLost*>(&eventData);
            LOGI("OPENXR: Events Lost: %d", eventsLost->lostEventCount);
            break;
        }
            // Log that an instance loss is pending and shutdown the application.
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
            XrEventDataInstanceLossPending* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData);
            LOGI("OPENXR: Instance Loss Pending at: %ld", instanceLossPending->lossTime);
            m_sessionRunning = false;
            m_applicationRunning = false;
            break;
        }
            // Log that the interaction profile has changed.
        case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
            XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData);
            LOGI("OPENXR: Interaction Profile changed for Session: %p", interactionProfileChanged->session);
            if (interactionProfileChanged->session != m_session) {
                LOGW("XrEventDataInteractionProfileChanged for unknown Session");
                break;
            }
            RecordCurrentBindings();
            break;
        }
            // Log that there's a reference space change pending.
        case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
            XrEventDataReferenceSpaceChangePending* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData);
            LOGI("OPENXR: Reference Space Change pending for Session: %p", referenceSpaceChangePending->session);
            if (referenceSpaceChangePending->session != m_session) {
                LOGW("XrEventDataReferenceSpaceChangePending for unknown Session");
                break;
            }
            break;
        }
            // Session State changes:
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
            XrEventDataSessionStateChanged* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
            if (sessionStateChanged->session != m_session) {
                LOGW("XrEventDataSessionStateChanged for unknown Session");
                break;
            }

            if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
                // SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
                XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                sessionBeginInfo.primaryViewConfigurationType = m_viewConfiguration;
                OPENXR_CHECK(xrBeginSession(m_session, &sessionBeginInfo), "Failed to begin Session.");
                m_sessionRunning = true;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
                // SessionState is stopping. End the XrSession.
                OPENXR_CHECK(xrEndSession(m_session), "Failed to end Session.");
                m_sessionRunning = false;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
                // SessionState is exiting. Exit the application.
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
    }
}

void OpenXr::PollActions()
{
    XrActiveActionSet activeActionSet{};
    activeActionSet.actionSet = m_actionSet;
    activeActionSet.subactionPath = XR_NULL_PATH;

    XrActionsSyncInfo actionsSyncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
    actionsSyncInfo.countActiveActionSets = 1;
    actionsSyncInfo.activeActionSets = &activeActionSet;
    OPENXR_CHECK(xrSyncActions(m_session, &actionsSyncInfo), "Failed to sync Actions.");

    XrHandsEvent handsEvent{};
    for (int i = 0; i < 2; ++i) {
        auto& handPoseState{ m_handPoseState[i] };
        auto& handPose{ m_handPose[i] };
        auto& squeezeState{ m_squeezeState[i] };
        auto& triggerState{ m_triggerState[i] };

        XrActionStateGetInfo poseActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        poseActionStateGetInfo.action = m_palmPoseAction;
        poseActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStatePose(m_session, &poseActionStateGetInfo, &handPoseState), "Failed to get Pose State.");
        if (handPoseState.isActive) {
            XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
            XrResult res = xrLocateSpace(m_handPoseSpace[i], m_localSpace, m_frameState.predictedDisplayTime, &spaceLocation);
            if (XR_UNQUALIFIED_SUCCESS(res) && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                handPose = spaceLocation.pose;
            } else {
                handPoseState.isActive = false;
            }
        }

        XrActionStateGetInfo squeezeActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        squeezeActionStateGetInfo.action = m_squeezeAction;
        squeezeActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateFloat(m_session, &squeezeActionStateGetInfo, &m_squeezeState[i]), "Failed to get Float State of squeeze action.");

        XrActionStateGetInfo triggerActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        triggerActionStateGetInfo.action = m_triggerAction;
        triggerActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateBoolean(m_session, &triggerActionStateGetInfo, &m_triggerState[i]), "Failed to get Boolean State of trigger action.");

        auto& handEvent{ handsEvent.hands[i] };
        handEvent.type = i == 0 ? HandType::LEFT : HandType::RIGHT;
        handEvent.active = handPoseState.isActive;
        handEvent.pose = prev::util::math::Pose{ { handPose.orientation.w, handPose.orientation.x, handPose.orientation.y, handPose.orientation.z }, { handPose.position.x, handPose.position.y, handPose.position.z } };
        handEvent.flags = {};
        handEvent.flags |= (squeezeState.isActive == XR_TRUE) ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
        handEvent.flags |= (triggerState.isActive == XR_TRUE && triggerState.currentState == XR_FALSE && triggerState.changedSinceLastSync == XR_TRUE) ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
        handEvent.squeeze = squeezeState.currentState;
    }
    prev::event::EventChannel::Post(handsEvent);
}

bool OpenXr::SuggestBindings()
{
    auto SuggestBindings = [](const XrInstance instance, const char* profilePath, const std::vector<XrActionSuggestedBinding>& bindings) -> bool {
        XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        interactionProfileSuggestedBinding.interactionProfile = ConvertStringToXrPath(instance, profilePath);
        interactionProfileSuggestedBinding.suggestedBindings = bindings.data();
        interactionProfileSuggestedBinding.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
        if (xrSuggestInteractionProfileBindings(instance, &interactionProfileSuggestedBinding) == XrResult::XR_SUCCESS) {
            return true;
        }
        LOGI("Failed to suggest bindings with %s", profilePath);
        return false;
    };

    bool anyOk{ false };
    anyOk |= SuggestBindings(m_xrInstance, "/interaction_profiles/khr/simple_controller", {
                                                                                              { m_triggerAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/left/input/select/click") },
                                                                                              { m_squeezeAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/right/input/select/click") },
                                                                                              { m_palmPoseAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/left/input/grip/pose") },
                                                                                              { m_palmPoseAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/right/input/grip/pose") },
                                                                                          });

    anyOk |= SuggestBindings(m_xrInstance, "/interaction_profiles/oculus/touch_controller", { { m_squeezeAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/left/input/squeeze/value") }, { m_squeezeAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/right/input/squeeze/value") }, { m_triggerAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/left/input/trigger/value") }, { m_triggerAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/right/input/trigger/value") }, { m_palmPoseAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/left/input/grip/pose") }, { m_palmPoseAction, ConvertStringToXrPath(m_xrInstance, "/user/hand/right/input/grip/pose") } });
    return anyOk;
}

void OpenXr::CreateActionPoses()
{
    auto CreateActionPoseSpace = [](const XrInstance instance, const XrSession session, const XrAction xrAction, const char* subactionPath = nullptr) -> XrSpace {
        const XrPosef xrPoseIdentity = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
        // Create frame of reference for a pose action
        XrActionSpaceCreateInfo actionSpaceCI{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
        actionSpaceCI.action = xrAction;
        actionSpaceCI.poseInActionSpace = xrPoseIdentity;
        if (subactionPath) {
            actionSpaceCI.subactionPath = ConvertStringToXrPath(instance, subactionPath);
        }

        XrSpace xrSpace{};
        OPENXR_CHECK(xrCreateActionSpace(session, &actionSpaceCI, &xrSpace), "Failed to create ActionSpace.");
        return xrSpace;
    };

    m_handPoseSpace[0] = CreateActionPoseSpace(m_xrInstance, m_session, m_palmPoseAction, "/user/hand/left");
    m_handPoseSpace[1] = CreateActionPoseSpace(m_xrInstance, m_session, m_palmPoseAction, "/user/hand/right");
}

void OpenXr::RecordCurrentBindings()
{
    if (m_session) {
        XrInteractionProfileState interactionProfile = { XR_TYPE_INTERACTION_PROFILE_STATE, 0, 0 };
        OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[0], &interactionProfile), "Failed to get profile.");
        if (interactionProfile.interactionProfile) {
            LOGI("user/hand/left ActiveProfile: %s", ConvertXrPathToString(m_xrInstance, interactionProfile.interactionProfile).c_str());
        }
        OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[1], &interactionProfile), "Failed to get profile.");
        if (interactionProfile.interactionProfile) {
            LOGI("user/hand/right ActiveProfile: %s", ConvertXrPathToString(m_xrInstance, interactionProfile.interactionProfile).c_str());
        }
    }
}
} // namespace prev::xr

#endif
