#include "OpenXrRender.h"

#ifdef ENABLE_XR

#include "../../util/VkUtils.h"

namespace prev::xr::render {
namespace {
    VkImageAspectFlags ToVkImageAspectFlags(const XrSwapchainUsageFlags xrUsageFlags)
    {
        if (xrUsageFlags & XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        } else {
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }
} // namespace

OpenXrRender::OpenXrRender(XrInstance instance, XrSystemId systemId)
    : m_instance{ instance }
    , m_systemId{ systemId }
{
    CreateViewConfigurationViews();
    CreateEnvironmentBlendModes();
}

OpenXrRender::~OpenXrRender()
{
    DestroyEnvironmentBlendModes();
    DestroyViewConfigurationViews();
}

void OpenXrRender::OnSessionCreate(XrSession session)
{
    m_session = session;

    CreateSwapchains();
}

void OpenXrRender::OnSessionDestroy()
{
    DestroySwapchains();

    m_session = XR_NULL_HANDLE;
}

void OpenXrRender::OnReferenceSpaceCreate(XrSpace space)
{
    m_localSpace = space;
}

void OpenXrRender::OnReferenceSpaceDestroy()
{
    m_localSpace = XR_NULL_HANDLE;
}

bool OpenXrRender::BeginFrame()
{
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
    OPENXR_CHECK(xrWaitFrame(m_session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

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
    uint32_t viewCount{ 0 };
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
    uint32_t colorImageIndex{ 0 };
    uint32_t depthImageIndex{ 0 };
    XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
    OPENXR_CHECK(xrAcquireSwapchainImage(m_colorSwapchainInfo.swapchain, &acquireInfo, &colorImageIndex), "Failed to acquire Image from the Color Swapchian");
    if(m_depthSwapchainInfo.swapchain) {
        OPENXR_CHECK(xrAcquireSwapchainImage(m_depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");
    }
    XrSwapchainImageWaitInfo waitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
    waitInfo.timeout = XR_INFINITE_DURATION;
    OPENXR_CHECK(xrWaitSwapchainImage(m_colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
    if(m_depthSwapchainInfo.swapchain) {
        OPENXR_CHECK(xrWaitSwapchainImage(m_depthSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Depth Swapchain");
    }
    // Get the width and height and construct the viewport and scissors.
    const int32_t width{ static_cast<int32_t>(m_viewConfigurationViews[0].recommendedImageRectWidth) };
    const int32_t height{ static_cast<int32_t>(m_viewConfigurationViews[0].recommendedImageRectHeight) };

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
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width = width;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height = height;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex = i; // Useful for multiview rendering.

        // depth layer
        if(m_depthSwapchainInfo.swapchain) {
            m_renderLayerInfo.layerProjectionViews[i].next = &m_renderLayerInfo.layerDepthInfos[i];

            m_renderLayerInfo.layerDepthInfos[i] = {XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR};
            m_renderLayerInfo.layerDepthInfos[i].subImage.swapchain = m_depthSwapchainInfo.swapchain;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.x = 0;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.y = 0;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.width = width;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.height = height;
            m_renderLayerInfo.layerDepthInfos[i].minDepth = m_minDepth;
            m_renderLayerInfo.layerDepthInfos[i].maxDepth = m_maxDepth;
            m_renderLayerInfo.layerDepthInfos[i].nearZ = m_nearClippingPlane;
            m_renderLayerInfo.layerDepthInfos[i].farZ = m_farClippingPlane;
        }
    }

    m_currentSwapchainIndex = colorImageIndex;

    return true;
}

bool OpenXrRender::EndFrame()
{
    // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
    XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
    OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
    if(m_depthSwapchainInfo.swapchain) {
        OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");
    }
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

void OpenXrRender::UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    m_graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
    m_graphicsBinding.instance = instance;
    m_graphicsBinding.physicalDevice = physicalDevice;
    m_graphicsBinding.device = device;
    m_graphicsBinding.queueFamilyIndex = queueFamilyIndex;
    m_graphicsBinding.queueIndex = queueIndex;
}

XrTime OpenXrRender::GetCurrentTime() const
{
    return m_frameState.predictedDisplayTime;
}

float OpenXrRender::GetCurrentDeltaTime() const
{
    return m_currentDeltaTime;
}

uint32_t OpenXrRender::GetCurrentSwapchainIndex() const
{
    return m_currentSwapchainIndex;
}

uint32_t OpenXrRender::GetViewCount() const
{
    return static_cast<uint32_t>(m_viewConfigurationViews.size());
}

std::vector<VkImage> OpenXrRender::GetColorImages() const
{
    return m_colorSwapchainInfo.images;
}

std::vector<VkImageView> OpenXrRender::GetColorImagesViews() const
{
    return m_colorSwapchainInfo.imageViews;
}

std::vector<VkImage> OpenXrRender::GetDepthImages() const
{
    return m_depthSwapchainInfo.images;
}

std::vector<VkImageView> OpenXrRender::GetDepthImagesViews() const
{
    return m_depthSwapchainInfo.imageViews;
}

VkExtent2D OpenXrRender::GetExtent() const
{
    return { m_viewConfigurationViews[0].recommendedImageRectWidth, m_viewConfigurationViews[0].recommendedImageRectHeight };
}

VkFormat OpenXrRender::GetColorFormat() const
{
    return m_preferredColorFormat;
}

VkFormat OpenXrRender::GetDepthFormat() const
{
    return m_preferredDepthFormat;
}

XrViewConfigurationType OpenXrRender::GetViewConfiguration() const
{
    return m_viewConfiguration;
}

const XrGraphicsBindingVulkanKHR& OpenXrRender::GetGraphicsBinding() const
{
    return m_graphicsBinding;
}

void OpenXrRender::OnEvent(const XrEventDataBuffer& evt)
{
}

void OpenXrRender::operator()(const XrCameraFeedbackEvent& event)
{
    m_nearClippingPlane = event.nearClippingPlane;
    m_farClippingPlane = event.fatClippingPlane;
    m_minDepth = event.minDepth;
    m_maxDepth = event.maxDepth;
}

void OpenXrRender::CreateViewConfigurationViews()
{
    // Gets the View Configuration Types. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationCount{ 0 };
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_instance, m_systemId, 0, &viewConfigurationCount, nullptr), "Failed to enumerate View Configurations.");
    m_viewConfigurations.resize(viewConfigurationCount);
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_instance, m_systemId, viewConfigurationCount, &viewConfigurationCount, m_viewConfigurations.data()), "Failed to enumerate View Configurations.");

    // Pick the first application supported View Configuration Type con supported by the hardware.
    for (const XrViewConfigurationType& viewConfiguration : m_preferredViewConfigurations) {
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
    uint32_t viewConfigurationViewCount{ 0 };
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_instance, m_systemId, m_viewConfiguration, 0, &viewConfigurationViewCount, nullptr), "Failed to enumerate ViewConfiguration Views.");
    m_viewConfigurationViews.resize(viewConfigurationViewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_instance, m_systemId, m_viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");

    if (viewConfigurationCount > MAX_VIEW_COUNT) {
        LOGE("OpenXR view configuration count > maxViewCount: %d > %d", viewConfigurationCount, MAX_VIEW_COUNT);
    }
}

void OpenXrRender::DestroyViewConfigurationViews()
{
    m_viewConfigurationViews.clear();
    m_viewConfigurations.clear();
    m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
}

void OpenXrRender::CreateEnvironmentBlendModes()
{
    // Retrieves the available blend modes. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t environmentBlendModeCount{ 0 };
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_instance, m_systemId, m_viewConfiguration, 0, &environmentBlendModeCount, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
    m_environmentBlendModes.resize(environmentBlendModeCount);
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_instance, m_systemId, m_viewConfiguration, environmentBlendModeCount, &environmentBlendModeCount, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

    // Pick the first application supported blend mode supported by the hardware.
    for (const XrEnvironmentBlendMode& environmentBlendMode : m_preferredEnvironmentBlendModes) {
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

void OpenXrRender::DestroyEnvironmentBlendModes()
{
    m_environmentBlendModes.clear();
    m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;
}

void OpenXrRender::CreateSwapchains()
{
    // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
    uint32_t formatCount{ 0 };
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &formatCount, nullptr), "Failed to enumerate Swapchain Formats");
    std::vector<int64_t> formats(formatCount);
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, formatCount, &formatCount, formats.data()), "Failed to enumerate Swapchain Formats");

    auto colorFormatIter = std::find(formats.begin(), formats.end(), m_preferredColorFormat);
    if (colorFormatIter == formats.cend()) {
        LOGE("Failed to find color format for Swapchain.");
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

    const XrViewConfigurationView& viewConfigurationView{ m_viewConfigurationViews[0] };
    const uint32_t viewCount{ static_cast<uint32_t>(m_viewConfigurationViews.size()) };

    m_colorSwapchainInfo = CreateSwapchain(viewConfigurationView, viewCount, m_preferredColorFormat, XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT);

#ifdef ENABLE_XR_DEPTH
    auto depthFormatIter = std::find(formats.begin(), formats.end(), m_preferredDepthFormat);
    if (depthFormatIter != formats.cend()) {
        m_depthSwapchainInfo = CreateSwapchain(viewConfigurationView, viewCount, m_preferredDepthFormat, XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    } else {
        LOGW("Failed to find depth format for Swapchain.");
    }
#endif
}

void OpenXrRender::DestroySwapchains()
{
    DestroySwapchain(m_depthSwapchainInfo);
    DestroySwapchain(m_colorSwapchainInfo);
}

OpenXrRender::SwapchainInfo OpenXrRender::CreateSwapchain(const XrViewConfigurationView& viewConfigurationView, const uint32_t viewCount, const VkFormat format, const XrSwapchainUsageFlags usageFlags)
{
    SwapchainInfo swapchainInfo{};

    XrSwapchainCreateInfo swapchainCreateInfo{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
    swapchainCreateInfo.createFlags = 0;
    swapchainCreateInfo.usageFlags = usageFlags;
    swapchainCreateInfo.format = format;
    swapchainCreateInfo.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount; // Use the recommended values from the XrViewConfigurationView.
    swapchainCreateInfo.width = viewConfigurationView.recommendedImageRectWidth;
    swapchainCreateInfo.height = viewConfigurationView.recommendedImageRectHeight;
    swapchainCreateInfo.faceCount = 1;
    swapchainCreateInfo.arraySize = viewCount;
    swapchainCreateInfo.mipCount = 1;

    OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCreateInfo, &swapchainInfo.swapchain), "Failed to create Swapchain");
    swapchainInfo.swapchainFormat = static_cast<VkFormat>(swapchainCreateInfo.format); // Save the swapchain format for later use.

    // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
    uint32_t swapchainImageCount{ 0 };
    OPENXR_CHECK(xrEnumerateSwapchainImages(swapchainInfo.swapchain, 0, &swapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");
    swapchainInfo.xrImages.resize(swapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
    XrSwapchainImageBaseHeader* swapchainImages{ reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainInfo.xrImages.data()) };
    OPENXR_CHECK(xrEnumerateSwapchainImages(swapchainInfo.swapchain, swapchainImageCount, &swapchainImageCount, swapchainImages), "Failed to enumerate Swapchain Images.");

    const VkImageAspectFlags vkImageAspectFlags{ ToVkImageAspectFlags(usageFlags) };
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        const auto image{ swapchainInfo.xrImages[i].image };
        const auto imageView{ prev::util::vk::CreateImageView(m_graphicsBinding.device, image, format, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1, vkImageAspectFlags, viewCount, 0) };
        swapchainInfo.images.push_back(image);
        swapchainInfo.imageViews.push_back(imageView);
    }
    return swapchainInfo;
}

void OpenXrRender::DestroySwapchain(SwapchainInfo& swapchainInfo)
{
    for (auto& imageView : swapchainInfo.imageViews) {
        vkDestroyImageView(m_graphicsBinding.device, imageView, VK_NULL_HANDLE);
    }
    OPENXR_CHECK(xrDestroySwapchain(swapchainInfo.swapchain), "Failed to destroy Color Swapchain");
    swapchainInfo = {};
}
} // namespace prev::xr::render

#endif