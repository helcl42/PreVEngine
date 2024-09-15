#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#include "OpenXRCommon.h"

#include <vector>
#include <unordered_map>

namespace prev::xr {
class OpenXR {
public:
    OpenXR();

    ~OpenXR();

public:
    std::vector<std::string> GetVulkanInstanceExtensions() const;

    std::vector<std::string> GetVulkanDeviceExtensions() const;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const;

    void InitializeGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, uint32_t queueFamilyIndex, uint32_t queueIndex);

    void DestroyGraphicsBinding();

    void CreateSession();

    void DestroySession();

    std::vector<VkImage> GetColorImages() const;

    std::vector<VkImageView> GetColorImagesViews() const;

    std::vector<VkImage> GetDepthImages() const;

    std::vector<VkImageView> GetDepthImagesViews() const;

    VkExtent2D GetExtent() const;

    VkFormat GetColorFormat() const;

    VkFormat GetDepthFormat() const;

    // TODO - handle predicted time correctly
    bool BeginFrame(uint32_t& outImageIndex);

    bool EndFrame();

    void PollEvents();

private:
    void CreateInstance();

    void DestroyInstance();

    void CreateDebugMessenger();

    void DestroyDebugMessenger();

    void GetInstanceProperties();

    void GetSystemID();

    void GetViewConfigurationViews();

    void GetEnvironmentBlendModes();

    // session context
    void CreateReferenceSpace();

    void DestroyReferenceSpace();

    void CreateSwapchains();

    void DestroySwapchains();

private:
    XrInstance m_xrInstance = XR_NULL_HANDLE;
    std::vector<const char *> m_activeAPILayers = {};
    std::vector<const char *> m_activeInstanceExtensions = {};
    std::vector<std::string> m_apiLayers = {};
    std::vector<std::string> m_instanceExtensions = {};

    XrDebugUtilsMessengerEXT m_debugUtilsMessenger = XR_NULL_HANDLE;

    XrSystemId m_systemID = {};
    XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

    const VkFormat m_preferredColorFormat = { VK_FORMAT_R8G8B8A8_UNORM };
    const VkFormat m_preferredDepthFormat = { VK_FORMAT_D32_SFLOAT };

    std::vector<XrViewConfigurationType> m_applicationViewConfigurations = {XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
    XrEnvironmentBlendMode m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

    XrGraphicsBindingVulkanKHR m_graphicsBinding{};

    XrSession m_session = {};
    XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;
    bool m_applicationRunning = true;
    bool m_sessionRunning = false;

    XrSpace m_localSpace = XR_NULL_HANDLE;

    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };
    SwapchainInfo m_colorSwapchainInfo = {};
    SwapchainInfo m_depthSwapchainInfo = {};

    enum class SwapchainType : uint8_t {
        COLOR,
        DEPTH
    };

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageVulkanKHR>>> swapchainImagesMap{};

    // per frame data
    struct RenderLayerInfo {
        XrTime predictedDisplayTime = 0;
        std::vector<XrCompositionLayerBaseHeader *> layers;
        XrCompositionLayerProjection layerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
        std::vector<XrCompositionLayerDepthInfoKHR> layerDepthInfos;
    };

    RenderLayerInfo m_renderLayerInfo = {};
    XrFrameState m_frameState = {};
};
}

#endif