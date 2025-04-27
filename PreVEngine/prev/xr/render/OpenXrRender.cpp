#include "OpenXrRender.h"

#ifdef ENABLE_XR

#include "../../util/VkUtils.h"

namespace prev::xr::render {
OpenXrRender::OpenXrRender(common::OpenXrContext& context)
    : m_context{ context }
{
}

OpenXrRender::~OpenXrRender()
{
    ShutDown();
}

void OpenXrRender::Init()
{
    GetViewConfigurationViews();
    GetEnvironmentBlendModes();
}

void OpenXrRender::ShutDown()
{
}


void OpenXrRender::OnSessionCreate()
{
    CreateSwapchains();
}

void OpenXrRender::OnSessionDestroy()
{
    DestroySwapchains();
}

void OpenXrRender::CreateSwapchains()
{
    // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
    uint32_t formatCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_context.session, 0, &formatCount, nullptr), "Failed to enumerate Swapchain Formats");
    std::vector<int64_t> formats(formatCount);
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_context.session, formatCount, &formatCount, formats.data()), "Failed to enumerate Swapchain Formats");

    auto colorFormatIter = std::find(formats.begin(), formats.end(), m_preferredColorFormat);
    if (colorFormatIter == formats.cend()) {
        LOGE("Failed to find color format for Swapchain.");
    }

    auto depthFormatIter = std::find(formats.begin(), formats.end(), m_preferredDepthFormat);
    if (depthFormatIter == formats.cend()) {
        LOGE("Failed to find depth format for Swapchain.");
    }

    bool coherentViews = m_context.viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
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

    // Color
    XrSwapchainCreateInfo colorSwapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO };
    colorSwapchainCreateInfo.createFlags = 0;
    colorSwapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
    colorSwapchainCreateInfo.format = m_preferredColorFormat;
    colorSwapchainCreateInfo.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount; // Use the recommended values from the XrViewConfigurationView.
    colorSwapchainCreateInfo.width = viewConfigurationView.recommendedImageRectWidth;
    colorSwapchainCreateInfo.height = viewConfigurationView.recommendedImageRectHeight;
    colorSwapchainCreateInfo.faceCount = 1;
    colorSwapchainCreateInfo.arraySize = viewCount;
    colorSwapchainCreateInfo.mipCount = 1;

    OPENXR_CHECK(xrCreateSwapchain(m_context.session, &colorSwapchainCreateInfo, &m_colorSwapchainInfo.swapchain), "Failed to create Color Swapchain");
    m_colorSwapchainInfo.swapchainFormat = static_cast<VkFormat>(colorSwapchainCreateInfo.format); // Save the swapchain format for later use.

    // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
    uint32_t colorSwapchainImageCount{ 0 };
    OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, 0, &colorSwapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");

    swapchainImagesMap[m_colorSwapchainInfo.swapchain].first = SwapchainType::COLOR;
    swapchainImagesMap[m_colorSwapchainInfo.swapchain].second.resize(colorSwapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
    XrSwapchainImageBaseHeader* colorSwapchainImages = reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImagesMap[m_colorSwapchainInfo.swapchain].second.data());

    OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, colorSwapchainImageCount, &colorSwapchainImageCount, colorSwapchainImages), "Failed to enumerate Color Swapchain Images.");

    for (uint32_t j = 0; j < colorSwapchainImageCount; ++j) {
        const auto image{ swapchainImagesMap[m_colorSwapchainInfo.swapchain].second[j].image };
        const auto imageView{ prev::util::vk::CreateImageView(m_context.graphicsBinding.device, image, m_preferredColorFormat, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1, VK_IMAGE_ASPECT_COLOR_BIT, viewCount, 0) };
        m_colorSwapchainInfo.images.push_back(image);
        m_colorSwapchainInfo.imageViews.push_back(imageView);
    }

    // Depth.
    XrSwapchainCreateInfo depthSwapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO };
    depthSwapchainCreateInfo.createFlags = 0;
    depthSwapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthSwapchainCreateInfo.format = m_preferredDepthFormat;
    depthSwapchainCreateInfo.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount; // Use the recommended values from the XrViewConfigurationView.
    depthSwapchainCreateInfo.width = viewConfigurationView.recommendedImageRectWidth;
    depthSwapchainCreateInfo.height = viewConfigurationView.recommendedImageRectHeight;
    depthSwapchainCreateInfo.faceCount = 1;
    depthSwapchainCreateInfo.arraySize = viewCount;
    depthSwapchainCreateInfo.mipCount = 1;
    OPENXR_CHECK(xrCreateSwapchain(m_context.session, &depthSwapchainCreateInfo, &m_depthSwapchainInfo.swapchain), "Failed to create Depth Swapchain");
    m_depthSwapchainInfo.swapchainFormat = static_cast<VkFormat>(depthSwapchainCreateInfo.format); // Save the swapchain format for later use.

    uint32_t depthSwapchainImageCount{ 0 };
    OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, 0, &depthSwapchainImageCount, nullptr), "Failed to enumerate Depth Swapchain Images.");

    swapchainImagesMap[m_depthSwapchainInfo.swapchain].first = SwapchainType::DEPTH;
    swapchainImagesMap[m_depthSwapchainInfo.swapchain].second.resize(depthSwapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
    XrSwapchainImageBaseHeader* depthSwapchainImages = reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImagesMap[m_depthSwapchainInfo.swapchain].second.data());

    OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, depthSwapchainImageCount, &depthSwapchainImageCount, depthSwapchainImages), "Failed to enumerate Depth Swapchain Images.");

    for (uint32_t j = 0; j < depthSwapchainImageCount; ++j) {
        const auto image{ swapchainImagesMap[m_depthSwapchainInfo.swapchain].second[j].image };
        const auto imageView{ prev::util::vk::CreateImageView(m_context.graphicsBinding.device, image, m_preferredDepthFormat, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1, VK_IMAGE_ASPECT_DEPTH_BIT, viewCount, 0) };
        m_depthSwapchainInfo.images.push_back(image);
        m_depthSwapchainInfo.imageViews.push_back(imageView);
    }
}

void OpenXrRender::DestroySwapchains()
{
    // Destroy the color and depth image views from GraphicsAPI.
    for (auto& imageView : m_colorSwapchainInfo.imageViews) {
        vkDestroyImageView(m_context.graphicsBinding.device, imageView, VK_NULL_HANDLE);
    }
    for (auto& imageView : m_depthSwapchainInfo.imageViews) {
        vkDestroyImageView(m_context.graphicsBinding.device, imageView, VK_NULL_HANDLE);
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

bool OpenXrRender::BeginFrame()
{
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
    OPENXR_CHECK(xrWaitFrame(m_context.session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

    XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
    OPENXR_CHECK(xrBeginFrame(m_context.session, &frameBeginInfo), "Failed to begin the XR Frame.");

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
    viewLocateInfo.viewConfigurationType = m_context.viewConfiguration;
    viewLocateInfo.displayTime = frameState.predictedDisplayTime;
    viewLocateInfo.space = m_context.localSpace;
    uint32_t viewCount = 0;
    XrResult result = xrLocateViews(m_context.session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
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
    OPENXR_CHECK(xrAcquireSwapchainImage(m_depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");

    XrSwapchainImageWaitInfo waitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
    waitInfo.timeout = XR_INFINITE_DURATION;
    OPENXR_CHECK(xrWaitSwapchainImage(m_colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
    OPENXR_CHECK(xrWaitSwapchainImage(m_depthSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Depth Swapchain");

    // Get the width and height and construct the viewport and scissors.
    const uint32_t& width{ m_viewConfigurationViews[0].recommendedImageRectWidth };
    const uint32_t& height{ m_viewConfigurationViews[0].recommendedImageRectHeight };

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

bool OpenXrRender::EndFrame()
{
    // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
    XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
    OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
    OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");

    m_renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    m_renderLayerInfo.layerProjection.space = m_context.localSpace;
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
    OPENXR_CHECK(xrEndFrame(m_context.session, &frameEndInfo), "Failed to end the XR Frame.");

    return true;
}

void OpenXrRender::UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    m_context.graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
    m_context.graphicsBinding.instance = instance;
    m_context.graphicsBinding.physicalDevice = physicalDevice;
    m_context.graphicsBinding.device = device;
    m_context.graphicsBinding.queueFamilyIndex = queueFamilyIndex;
    m_context.graphicsBinding.queueIndex = queueIndex;
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
    return  m_currentSwapchainIndex;
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

void OpenXrRender::OnOpenXrEvent(const XrEventDataBuffer& evt)
{
}

void OpenXrRender::operator()(const XrCameraFeedbackEvent& event)
{
    m_nearClippingPlane = event.nearClippingPlane;
    m_farClippingPlane = event.fatClippingPlane;
    m_minDepth = event.minDepth;
    m_maxDepth = event.maxDepth;
}

void OpenXrRender::GetViewConfigurationViews()
{
    // Gets the View Configuration Types. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationCount{ 0 };
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_context.instance, m_context.systemId, 0, &viewConfigurationCount, nullptr), "Failed to enumerate View Configurations.");
    m_viewConfigurations.resize(viewConfigurationCount);
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_context.instance, m_context.systemId, viewConfigurationCount, &viewConfigurationCount, m_viewConfigurations.data()), "Failed to enumerate View Configurations.");

    // Pick the first application supported View Configuration Type con supported by the hardware.
    for (const XrViewConfigurationType& viewConfiguration : m_applicationViewConfigurations) {
        if (std::find(m_viewConfigurations.begin(), m_viewConfigurations.end(), viewConfiguration) != m_viewConfigurations.end()) {
            m_context.viewConfiguration = viewConfiguration;
            break;
        }
    }
    if (m_context.viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
        LOGE("Failed to find a view configuration type. Defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO.");
        m_context.viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    }

    // Gets the View Configuration Views. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationViewCount{ 0 };
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_context.instance, m_context.systemId, m_context.viewConfiguration, 0, &viewConfigurationViewCount, nullptr), "Failed to enumerate ViewConfiguration Views.");
    m_viewConfigurationViews.resize(viewConfigurationViewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_context.instance, m_context.systemId, m_context.viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");

    if (viewConfigurationCount > MAX_VIEW_COUNT) {
        LOGE("OpenXR view configuration count > maxViewCount: %d > %d", viewConfigurationCount, MAX_VIEW_COUNT);
    }
}

void OpenXrRender::GetEnvironmentBlendModes()
{
    // Retrieves the available blend modes. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t environmentBlendModeCount{ 0 };
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_context.instance, m_context.systemId, m_context.viewConfiguration, 0, &environmentBlendModeCount, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
    m_environmentBlendModes.resize(environmentBlendModeCount);
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_context.instance, m_context.systemId, m_context.viewConfiguration, environmentBlendModeCount, &environmentBlendModeCount, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

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
}

#endif