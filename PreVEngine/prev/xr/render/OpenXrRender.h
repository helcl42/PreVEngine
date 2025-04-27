#ifndef __OPEN_XR_RENDER_H__
#define __OPEN_XR_RENDER_H__

#ifdef ENABLE_XR

#include "../common/OpenXrCommon.h"
#include "../common/OpenXrContext.h"
#include "../common/IOpenXrEventObserver.h"

#include "../XrEvents.h"

#include <unordered_map>

#include <prev/event/EventHandler.h>

namespace prev::xr::render {
class OpenXrRender final : public common::IOpenXrEventObserver {
public:
    explicit OpenXrRender(common::OpenXrContext& context);

    ~OpenXrRender();

public:
    void Init();

    void ShutDown();

    void OnSessionCreate();

    void OnSessionDestroy();

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

public:
    void OnOpenXrEvent(const XrEventDataBuffer& evt) override;

public:
    void operator()(const XrCameraFeedbackEvent& event);

private:
    void GetViewConfigurationViews();

    void GetEnvironmentBlendModes();

    void CreateSwapchains();

    void DestroySwapchains();

private:
    common::OpenXrContext& m_context;

    const VkFormat m_preferredColorFormat{ VK_FORMAT_R8G8B8A8_UNORM };
    const VkFormat m_preferredDepthFormat{ VK_FORMAT_D32_SFLOAT };

    std::vector<XrViewConfigurationType> m_applicationViewConfigurations{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes{ XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE };
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes{};
    XrEnvironmentBlendMode m_environmentBlendMode{ XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM };

    struct SwapchainInfo {
        XrSwapchain swapchain{ XR_NULL_HANDLE };
        VkFormat swapchainFormat{ VK_FORMAT_UNDEFINED };
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };
    SwapchainInfo m_colorSwapchainInfo{};
    SwapchainInfo m_depthSwapchainInfo{};

    enum class SwapchainType : uint8_t {
        COLOR,
        DEPTH
    };

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageVulkanKHR>>> swapchainImagesMap;

    // per frame data
    struct RenderLayerInfo {
        XrTime predictedDisplayTime{ 0 };
        std::vector<XrCompositionLayerBaseHeader*> layers;
        XrCompositionLayerProjection layerProjection{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
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
}

#endif

#endif