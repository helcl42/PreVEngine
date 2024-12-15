#ifndef __OPEN_XR_H__
#define __OPEN_XR_H__

#include "OpenXrCommon.h"
#include "XrEvents.h"

#include "../event/EventHandler.h"

#include <vector>
#include <unordered_map>

namespace prev::xr {
class OpenXr {
public:
    OpenXr();

    ~OpenXr();

public:
    std::vector<std::string> GetVulkanInstanceExtensions() const;

    std::vector<std::string> GetVulkanDeviceExtensions() const;

    VkPhysicalDevice GetPhysicalDevice(VkInstance instance) const;

    void InitializeGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

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

    uint32_t GetViewCount() const;

    uint32_t GetCurrentSwapchainIndex() const;

    float GetCurrentDeltaTime() const;

    void PollEvents();

    void PollActions();

    bool BeginFrame();

    bool EndFrame();

public:
    void operator() (const XrCameraFeedbackEvent& event);

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

    void CreateActionSet();

    void DestroyActionSet();

    void AttachActionSet();

    void DetachActionSet();

    void CreateActionPoses();

    bool SuggestBindings();

    void RecordCurrentBindings();

private:
    XrInstance m_xrInstance = { XR_NULL_HANDLE };
    std::vector<const char *> m_activeAPILayers = {};
    std::vector<const char *> m_activeInstanceExtensions = {};
    std::vector<std::string> m_apiLayers = {};
    std::vector<std::string> m_instanceExtensions = {};

    XrDebugUtilsMessengerEXT m_debugUtilsMessenger = { XR_NULL_HANDLE };

    XrSystemId m_systemID = {};
    XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

    const VkFormat m_preferredColorFormat = { VK_FORMAT_R8G8B8A8_UNORM };
    const VkFormat m_preferredDepthFormat = { VK_FORMAT_D32_SFLOAT };

    std::vector<XrViewConfigurationType> m_applicationViewConfigurations = {XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    XrViewConfigurationType m_viewConfiguration = { XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM };
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
    XrEnvironmentBlendMode m_environmentBlendMode = { XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM };

    XrGraphicsBindingVulkanKHR m_graphicsBinding = {};

    XrSession m_session = {};
    XrSessionState m_sessionState = { XR_SESSION_STATE_UNKNOWN };
    bool m_applicationRunning{ true };
    bool m_sessionRunning{ false };

    XrSpace m_localSpace = { XR_NULL_HANDLE };

    struct SwapchainInfo {
        XrSwapchain swapchain = { XR_NULL_HANDLE };
        VkFormat swapchainFormat = { VK_FORMAT_UNDEFINED };
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
        XrTime predictedDisplayTime{ 0 };
        std::vector<XrCompositionLayerBaseHeader *> layers;
        XrCompositionLayerProjection layerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
        std::vector<XrCompositionLayerDepthInfoKHR> layerDepthInfos;
    };

    uint32_t m_currentSwapchainIndex{ 0 };
    RenderLayerInfo m_renderLayerInfo{};
    XrFrameState m_frameState{};
    float m_currentDeltaTime{ 0.0 };

    float m_nearClippingPlane{ 0.1f };
    float m_farClippingPlane{ 300.0f };
    float m_minDepth{ 0.0f };
    float m_maxDepth{ 1.0f };

    // inputs
    XrActionSet m_actionSet{};

    XrAction m_squeezeAction{};
    XrActionStateFloat m_squeezeState[2] = {{XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT}};

    XrAction m_triggerAction{};
    XrActionStateBoolean m_triggerState[2] = {{XR_TYPE_ACTION_STATE_BOOLEAN}, {XR_TYPE_ACTION_STATE_BOOLEAN}};

    XrAction m_palmPoseAction{};
    XrPath m_handPaths[2] = {0, 0};
    XrSpace m_handPoseSpace[2];
    XrActionStatePose m_handPoseState[2] = {{XR_TYPE_ACTION_STATE_POSE}, {XR_TYPE_ACTION_STATE_POSE}};
    XrPosef m_handPose[2] = {
            {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
            {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}};
private:
    prev::event::EventHandler<OpenXr, XrCameraFeedbackEvent> m_cameraFeedbackHandler{ *this };
};
}

#endif