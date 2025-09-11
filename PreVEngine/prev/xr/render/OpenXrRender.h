#ifndef __OPEN_XR_RENDER_H__
#define __OPEN_XR_RENDER_H__

#ifdef ENABLE_XR

#include "../XrEvents.h"
#include "../common/IOpenXrEventObserver.h"
#include "../util/OpenXRUtil.h"

#include <unordered_map>

#include <prev/event/EventHandler.h>

namespace prev::xr::render {
class OpenXrRender final : public common::IOpenXrEventObserver {
public:
    OpenXrRender(XrInstance instance, XrSystemId systemId);

    ~OpenXrRender();

public:
    void OnSessionCreate(XrSession session);

    void OnSessionDestroy();

    void OnReferenceSpaceCreate(XrSpace space);

    void OnReferenceSpaceDestroy();

    bool BeginFrame();

    bool EndFrame();

    void UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

    XrTime GetCurrentTime() const;

    float GetCurrentDeltaTime() const;

    uint32_t GetCurrentSwapchainIndex() const;

    uint32_t GetViewCount() const;

    std::vector<VkImage> GetColorImages() const;

    std::vector<VkImageView> GetColorImagesViews() const;

    std::vector<VkImage> GetDepthImages() const;

    std::vector<VkImageView> GetDepthImagesViews() const;

    VkExtent2D GetExtent() const;

    VkFormat GetColorFormat() const;

    VkFormat GetDepthFormat() const;

    XrViewConfigurationType GetViewConfiguration() const;

    const XrGraphicsBindingVulkanKHR& GetGraphicsBinding() const;

public:
    void OnEvent(const XrEventDataBuffer& evt) override;

public:
    void operator()(const XrCameraFeedbackEvent& event);

private:
    struct SwapchainInfo;

private:
    void CreateViewConfigurationViews();

    void DestroyViewConfigurationViews();

    void CreateEnvironmentBlendModes();

    void DestroyEnvironmentBlendModes();

    void CreateSwapchains();

    void DestroySwapchains();

    SwapchainInfo CreateSwapchain(const XrViewConfigurationView& viewConfigurationView, const uint32_t viewCount, const VkFormat format, const XrSwapchainUsageFlags usageFlags);

    void DestroySwapchain(SwapchainInfo& swapchainInfo);

private:
    XrInstance m_instance{ XR_NULL_HANDLE };
    XrSystemId m_systemId{ XR_NULL_SYSTEM_ID };

    const std::vector<XrViewConfigurationType> m_preferredViewConfigurations{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
    XrViewConfigurationType m_viewConfiguration{ XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM };
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    const std::vector<XrEnvironmentBlendMode> m_preferredEnvironmentBlendModes{ XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE };
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes{};
    XrEnvironmentBlendMode m_environmentBlendMode{ XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM };

    XrGraphicsBindingVulkanKHR m_graphicsBinding{};

    XrSession m_session{ XR_NULL_HANDLE };
    XrSpace m_localSpace{ XR_NULL_HANDLE };

    const VkFormat m_preferredColorFormat{ VK_FORMAT_R8G8B8A8_UNORM };
    const VkFormat m_preferredDepthFormat{ VK_FORMAT_D32_SFLOAT };

    struct SwapchainInfo {
        XrSwapchain swapchain{ XR_NULL_HANDLE };
        VkFormat swapchainFormat{ VK_FORMAT_UNDEFINED };
        std::vector<XrSwapchainImageVulkanKHR> xrImages;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };
    SwapchainInfo m_colorSwapchainInfo{};
    SwapchainInfo m_depthSwapchainInfo{};

    // per frame data
    struct RenderLayerInfo {
        XrTime predictedDisplayTime{ 0 };
        std::vector<XrCompositionLayerBaseHeader*> layers;
        XrCompositionLayerProjection layerProjection{};
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

private:
    prev::event::EventHandler<OpenXrRender, XrCameraFeedbackEvent> m_cameraFeedbackHandler{ *this };
};
} // namespace prev::xr::render

#endif

#endif