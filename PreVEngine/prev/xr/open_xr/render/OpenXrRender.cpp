#include "OpenXrRender.h"

#ifdef ENABLE_OPENXR

#include "../util/OpenXrUtils.h"

namespace prev::xr::open_xr::render {
namespace {
    GfxFormat VkFormatToGfxFormat(VkFormat format)
    {
        switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return GFX_FORMAT_R8G8B8A8_UNORM;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return GFX_FORMAT_R8G8B8A8_UNORM_SRGB;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return GFX_FORMAT_B8G8R8A8_UNORM;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return GFX_FORMAT_B8G8R8A8_UNORM_SRGB;
        case VK_FORMAT_D16_UNORM:
            return GFX_FORMAT_DEPTH16_UNORM;
        case VK_FORMAT_D32_SFLOAT:
            return GFX_FORMAT_DEPTH32_FLOAT;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return GFX_FORMAT_DEPTH24_PLUS_STENCIL8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return GFX_FORMAT_DEPTH32_FLOAT_STENCIL8;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return GFX_FORMAT_R16G16B16A16_FLOAT;
        default:
            return GFX_FORMAT_UNDEFINED;
        }
    }

    // XR_FB_passthrough entry points (extension functions must be loaded via xrGetInstanceProcAddr).
    PFN_xrCreatePassthroughFB xrCreatePassthroughFB{};
    PFN_xrDestroyPassthroughFB xrDestroyPassthroughFB{};
    PFN_xrCreatePassthroughLayerFB xrCreatePassthroughLayerFB{};
    PFN_xrDestroyPassthroughLayerFB xrDestroyPassthroughLayerFB{};
    PFN_xrPassthroughLayerResumeFB xrPassthroughLayerResumeFB{};
    PFN_xrPassthroughLayerPauseFB xrPassthroughLayerPauseFB{};

    bool LoadPassthroughApi(XrInstance instance)
    {
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrCreatePassthroughFB", (PFN_xrVoidFunction*)&xrCreatePassthroughFB), "Failed to get xrCreatePassthroughFB.");
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrDestroyPassthroughFB", (PFN_xrVoidFunction*)&xrDestroyPassthroughFB), "Failed to get xrDestroyPassthroughFB.");
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrCreatePassthroughLayerFB", (PFN_xrVoidFunction*)&xrCreatePassthroughLayerFB), "Failed to get xrCreatePassthroughLayerFB.");
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrDestroyPassthroughLayerFB", (PFN_xrVoidFunction*)&xrDestroyPassthroughLayerFB), "Failed to get xrDestroyPassthroughLayerFB.");
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrPassthroughLayerResumeFB", (PFN_xrVoidFunction*)&xrPassthroughLayerResumeFB), "Failed to get xrPassthroughLayerResumeFB.");
        OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrPassthroughLayerPauseFB", (PFN_xrVoidFunction*)&xrPassthroughLayerPauseFB), "Failed to get xrPassthroughLayerPauseFB.");
        return xrCreatePassthroughFB && xrDestroyPassthroughFB && xrCreatePassthroughLayerFB && xrDestroyPassthroughLayerFB && xrPassthroughLayerResumeFB && xrPassthroughLayerPauseFB;
    }
} // namespace

OpenXrRender::OpenXrRender(XrInstance instance, XrSystemId systemId, bool passthroughSupported, bool passthroughEnabled, bool colorManaged)
    : m_instance{ instance }
    , m_systemId{ systemId }
    , m_passthroughSupported{ passthroughSupported }
    , m_passthroughEnabled{ passthroughSupported && passthroughEnabled }
    , m_colorFormat{ colorManaged ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM }
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
    if (m_passthroughSupported) {
        CreatePassthrough();
    }
}

void OpenXrRender::OnSessionDestroy()
{
    DestroyPassthrough();
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
    XrFrameState frameState{ prev::xr::open_xr::util::CreateStruct<XrFrameState>(XR_TYPE_FRAME_STATE) };
    XrFrameWaitInfo frameWaitInfo{ prev::xr::open_xr::util::CreateStruct<XrFrameWaitInfo>(XR_TYPE_FRAME_WAIT_INFO) };
    OPENXR_CHECK(xrWaitFrame(m_session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

    XrFrameBeginInfo frameBeginInfo{ prev::xr::open_xr::util::CreateStruct<XrFrameBeginInfo>(XR_TYPE_FRAME_BEGIN_INFO) };
    OPENXR_CHECK(xrBeginFrame(m_session, &frameBeginInfo), "Failed to begin the XR Frame.");

    if (m_frameState.predictedDisplayTime == 0) {
        m_currentDeltaTime = 0.0;
    } else {
        m_currentDeltaTime = static_cast<float>(frameState.predictedDisplayTime - m_frameState.predictedDisplayTime) * 1e-9f;
    }
    m_frameState = frameState;

    if (!frameState.shouldRender) {
        // Submit empty frame to keep the session alive (e.g., when Oculus overlay is active)
        XrFrameEndInfo frameEndInfo{ prev::xr::open_xr::util::CreateStruct<XrFrameEndInfo>(XR_TYPE_FRAME_END_INFO) };
        frameEndInfo.displayTime = frameState.predictedDisplayTime;
        frameEndInfo.environmentBlendMode = m_environmentBlendMode;
        frameEndInfo.layerCount = 0;
        frameEndInfo.layers = nullptr;
        OPENXR_CHECK(xrEndFrame(m_session, &frameEndInfo), "Failed to end the XR Frame.");
        return false;
    }

    // Locate the views from the view configuration within the (reference) space at the display time.
    XrViewState viewState{ prev::xr::open_xr::util::CreateStruct<XrViewState>(XR_TYPE_VIEW_STATE) }; // Will contain information on whether the position and/or orientation is valid and/or tracked.
    XrViewLocateInfo viewLocateInfo{ prev::xr::open_xr::util::CreateStruct<XrViewLocateInfo>(XR_TYPE_VIEW_LOCATE_INFO) };
    viewLocateInfo.viewConfigurationType = m_viewConfiguration;
    viewLocateInfo.displayTime = frameState.predictedDisplayTime;
    viewLocateInfo.space = m_localSpace;

    std::vector<XrView> views(static_cast<uint32_t>(m_viewConfigurationViews.size()), prev::xr::open_xr::util::CreateStruct<XrView>(XR_TYPE_VIEW));

    uint32_t viewCount{ 0 };
    if (XR_FAILED(xrLocateViews(m_session, &viewLocateInfo, &viewState, static_cast<uint32_t>(m_viewConfigurationViews.size()), &viewCount, views.data()))) {
        LOGE("Failed to query view count.");
        return false;
    }

    if (viewCount != static_cast<uint32_t>(m_viewConfigurationViews.size())) {
        views.resize(viewCount, prev::xr::open_xr::util::CreateStruct<XrView>(XR_TYPE_VIEW));
        if (XR_FAILED(xrLocateViews(m_session, &viewLocateInfo, &viewState, viewCount, &viewCount, views.data()))) {
            LOGE("Failed to locate Views.");
            return false;
        }
    }

    CameraEvent event{};
    const uint32_t maxEyes{ static_cast<uint32_t>(MAX_VIEW_COUNT_VALUE) };
    const uint32_t cameraViewCount{ viewCount < maxEyes ? viewCount : maxEyes };
    if (viewCount > maxEyes) {
        LOGW("OpenXR reported %u views but MAX_VIEW_COUNT is %u; truncating the camera event.", viewCount, maxEyes);
    }
    for (uint32_t i = 0; i < cameraViewCount; ++i) {
        const auto& view{ views[i] };
        event.poses[i] = prev::util::math::Pose{ { view.pose.orientation.w, view.pose.orientation.x, view.pose.orientation.y, view.pose.orientation.z }, { view.pose.position.x, view.pose.position.y, view.pose.position.z } };
        event.fovs[i] = prev::util::math::Fov{ view.fov.angleLeft, view.fov.angleRight, view.fov.angleUp, view.fov.angleDown };
    }
    event.count = cameraViewCount;
    prev::event::EventChannel::Post(event);

    m_renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;
    m_renderLayerInfo.layerProjection = prev::xr::open_xr::util::CreateStruct<XrCompositionLayerProjection>(XR_TYPE_COMPOSITION_LAYER_PROJECTION);
    m_renderLayerInfo.layerProjectionViews.resize(viewCount, prev::xr::open_xr::util::CreateStruct<XrCompositionLayerProjectionView>(XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW));
    m_renderLayerInfo.layerDepthInfos.resize(viewCount, prev::xr::open_xr::util::CreateStruct<XrCompositionLayerDepthInfoKHR>(XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR));

    // Acquire and wait for an image from the swapchains.
    // Get the image index of an image in the swapchains.
    // The timeout is infinite.
    uint32_t colorImageIndex{ 0 };
    uint32_t depthImageIndex{ 0 };
    XrSwapchainImageAcquireInfo acquireInfo{ prev::xr::open_xr::util::CreateStruct<XrSwapchainImageAcquireInfo>(XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO) };
    OPENXR_CHECK(xrAcquireSwapchainImage(m_colorSwapchainInfo.swapchain, &acquireInfo, &colorImageIndex), "Failed to acquire Image from the Color Swapchian");
    if (m_depthSwapchainInfo.swapchain) {
        OPENXR_CHECK(xrAcquireSwapchainImage(m_depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");
    }
    XrSwapchainImageWaitInfo waitInfo{ prev::xr::open_xr::util::CreateStruct<XrSwapchainImageWaitInfo>(XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO) };
    waitInfo.timeout = XR_INFINITE_DURATION;
    OPENXR_CHECK(xrWaitSwapchainImage(m_colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
    if (m_depthSwapchainInfo.swapchain) {
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
        m_renderLayerInfo.layerProjectionViews[i] = { prev::xr::open_xr::util::CreateStruct<XrCompositionLayerProjectionView>(XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW) };
        m_renderLayerInfo.layerProjectionViews[i].pose = views[i].pose;
        m_renderLayerInfo.layerProjectionViews[i].fov = views[i].fov;
        m_renderLayerInfo.layerProjectionViews[i].subImage.swapchain = m_colorSwapchainInfo.swapchain;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.x = 0;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.y = 0;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width = width;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height = height;
        m_renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex = i; // Useful for multiview rendering.

        // depth layer
        if (m_depthSwapchainInfo.swapchain) {
            m_renderLayerInfo.layerDepthInfos[i] = { prev::xr::open_xr::util::CreateStruct<XrCompositionLayerDepthInfoKHR>(XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR) };
            m_renderLayerInfo.layerDepthInfos[i].subImage.swapchain = m_depthSwapchainInfo.swapchain;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.x = 0;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.offset.y = 0;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.width = width;
            m_renderLayerInfo.layerDepthInfos[i].subImage.imageRect.extent.height = height;
            m_renderLayerInfo.layerDepthInfos[i].minDepth = m_minDepth;
            m_renderLayerInfo.layerDepthInfos[i].maxDepth = m_maxDepth;
            m_renderLayerInfo.layerDepthInfos[i].nearZ = m_nearClippingPlane;
            m_renderLayerInfo.layerDepthInfos[i].farZ = m_farClippingPlane;

            m_renderLayerInfo.layerProjectionViews[i].next = &m_renderLayerInfo.layerDepthInfos[i];
        }
    }

    m_currentSwapchainIndex = colorImageIndex;

    return true;
}

bool OpenXrRender::EndFrame()
{
    // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
    XrSwapchainImageReleaseInfo releaseInfo{ prev::xr::open_xr::util::CreateStruct<XrSwapchainImageReleaseInfo>(XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO) };
    OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
    if (m_depthSwapchainInfo.swapchain) {
        OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");
    }
    m_renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    m_renderLayerInfo.layerProjection.space = m_localSpace;
    m_renderLayerInfo.layerProjection.viewCount = static_cast<uint32_t>(m_renderLayerInfo.layerProjectionViews.size());
    m_renderLayerInfo.layerProjection.views = m_renderLayerInfo.layerProjectionViews.data();

    m_renderLayerInfo.layers.clear();
    if (m_passthroughEnabled && m_passthroughLayer != XR_NULL_HANDLE) {
        // Passthrough composites beneath the projection layer; the projection's SOURCE_ALPHA flag lets it through.
        m_passthroughCompositionLayer = { prev::xr::open_xr::util::CreateStruct<XrCompositionLayerPassthroughFB>(XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB) };
        m_passthroughCompositionLayer.flags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        m_passthroughCompositionLayer.layerHandle = m_passthroughLayer;
        m_renderLayerInfo.layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&m_passthroughCompositionLayer));
    }
    m_renderLayerInfo.layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&m_renderLayerInfo.layerProjection));

    // Tell OpenXR that we are finished with this frame; specifying its display time, environment blending and layers.
    XrFrameEndInfo frameEndInfo{ prev::xr::open_xr::util::CreateStruct<XrFrameEndInfo>(XR_TYPE_FRAME_END_INFO) };
    frameEndInfo.displayTime = m_frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = m_environmentBlendMode;
    frameEndInfo.layerCount = static_cast<uint32_t>(m_renderLayerInfo.layers.size());
    frameEndInfo.layers = m_renderLayerInfo.layers.data();
    OPENXR_CHECK(xrEndFrame(m_session, &frameEndInfo), "Failed to end the XR Frame.");

    return true;
}

void OpenXrRender::UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    void* nativeInstance{};
    gfxInstanceGetNativeHandle(instance, &nativeInstance);

    void* nativeAdapter{};
    gfxAdapterGetNativeHandle(adapter, &nativeAdapter);

    void* nativeDevice{};
    gfxDeviceGetNativeHandle(device, &nativeDevice);

    m_graphicsBinding = { prev::xr::open_xr::util::CreateStruct<XrGraphicsBindingVulkanKHR>(XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR) };
    m_graphicsBinding.instance = static_cast<VkInstance>(nativeInstance);
    m_graphicsBinding.physicalDevice = static_cast<VkPhysicalDevice>(nativeAdapter);
    m_graphicsBinding.device = static_cast<VkDevice>(nativeDevice);
    m_graphicsBinding.queueFamilyIndex = queueFamilyIndex;
    m_graphicsBinding.queueIndex = queueIndex;

    m_gfxDevice = device;
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

uint32_t OpenXrRender::GetImageCount() const
{
    return static_cast<uint32_t>(m_colorSwapchainInfo.textures.size());
}

GfxTexture OpenXrRender::GetColorImage(uint32_t index) const
{
    return index < m_colorSwapchainInfo.textures.size() ? m_colorSwapchainInfo.textures[index] : GfxTexture{};
}

GfxTexture OpenXrRender::GetDepthImage(uint32_t index) const
{
    return index < m_depthSwapchainInfo.textures.size() ? m_depthSwapchainInfo.textures[index] : GfxTexture{};
}

bool OpenXrRender::HasDepthImages() const
{
    return !m_depthSwapchainInfo.textures.empty();
}

GfxExtent2D OpenXrRender::GetExtent() const
{
    return { m_viewConfigurationViews[0].recommendedImageRectWidth, m_viewConfigurationViews[0].recommendedImageRectHeight };
}

GfxFormat OpenXrRender::GetColorFormat() const
{
    return VkFormatToGfxFormat(m_colorFormat);
}

GfxFormat OpenXrRender::GetDepthFormat() const
{
    return VkFormatToGfxFormat(m_preferredDepthFormat);
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

void OpenXrRender::operator()(const CameraFeedbackEvent& event)
{
    m_nearClippingPlane = event.nearClippingPlane;
    m_farClippingPlane = event.fatClippingPlane;
    m_minDepth = event.minDepth;
    m_maxDepth = event.maxDepth;
}

void OpenXrRender::operator()(const XrPassthroughChangeRequestEvent& event)
{
    SetPassthroughEnabled(event.enabled);
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
    m_viewConfigurationViews.resize(viewConfigurationViewCount, prev::xr::open_xr::util::CreateStruct<XrViewConfigurationView>(XR_TYPE_VIEW_CONFIGURATION_VIEW));
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_instance, m_systemId, m_viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");

    if (viewConfigurationCount > MAX_VIEW_COUNT_VALUE) {
        LOGE("OpenXR view configuration count > MAX_VIEW_COUNT (eyes): %d > %d", viewConfigurationCount, MAX_VIEW_COUNT_VALUE);
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

    // Use the preferred color format if the runtime offers it; otherwise fall back to a supported color
    // format rather than forcing an unsupported one (which would fail xrCreateSwapchain). If the fallback
    // isn't sRGB, XrEngineImpl reconciles colorManaged to gamma passthrough so content stays consistent.
    // If the runtime lacks the preferred format, fall back to a supported one (forcing it fails
    // xrCreateSwapchain). A non-sRGB fallback is reconciled to gamma passthrough by XrEngineImpl.
    if (std::find(formats.begin(), formats.end(), static_cast<int64_t>(m_colorFormat)) == formats.cend()) {
        LOGW("OpenXR runtime does not offer the preferred color format; selecting a supported fallback");
        const VkFormat colorFallbacks[]{ VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB };
        bool found = false;
        for (const VkFormat candidate : colorFallbacks) {
            if (std::find(formats.begin(), formats.end(), static_cast<int64_t>(candidate)) != formats.cend()) {
                m_colorFormat = candidate;
                found = true;
                break;
            }
        }
        if (!found) {
            LOGE("No supported color swapchain format found; xrCreateSwapchain may fail");
        }
    }

    bool coherentViews = m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    for (const XrViewConfigurationView& viewConfigurationView : m_viewConfigurationViews) {
        coherentViews &= m_viewConfigurationViews[0].recommendedImageRectWidth == viewConfigurationView.recommendedImageRectWidth;
        coherentViews &= m_viewConfigurationViews[0].recommendedImageRectHeight == viewConfigurationView.recommendedImageRectHeight;
    }
    if (!coherentViews) {
        LOGE("The views are not coherent. Unable to create a single Swapchain.");
    }

    const XrViewConfigurationView& viewConfigurationView{ m_viewConfigurationViews[0] };
    // The OpenXR color/depth swapchain holds one array layer per XR eye, independent of whether the engine
    // fills them via multiview (one pass, SV_ViewID) or per-eye (one pass per eye into layer[i]).
    const uint32_t viewCount{ static_cast<uint32_t>(m_viewConfigurationViews.size()) };

    m_colorSwapchainInfo = CreateSwapchain(viewConfigurationView, viewCount, m_colorFormat, XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT);

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

    XrSwapchainCreateInfo swapchainCreateInfo{ prev::xr::open_xr::util::CreateStruct<XrSwapchainCreateInfo>(XR_TYPE_SWAPCHAIN_CREATE_INFO) };
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
    swapchainInfo.xrImages.resize(swapchainImageCount, prev::xr::open_xr::util::CreateStruct<XrSwapchainImageVulkanKHR>(XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR));
    XrSwapchainImageBaseHeader* swapchainImages{ reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainInfo.xrImages.data()) };
    OPENXR_CHECK(xrEnumerateSwapchainImages(swapchainInfo.swapchain, swapchainImageCount, &swapchainImageCount, swapchainImages), "Failed to enumerate Swapchain Images.");

    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        const auto image{ swapchainInfo.xrImages[i].image };

        GfxTexture texture{};
        GfxTextureImportDescriptor importDesc{};
        importDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_IMPORT_DESCRIPTOR;
        importDesc.label = (usageFlags & XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? "XrDepthImage" : "XrColorImage";
        importDesc.nativeHandle = static_cast<void*>(image);
        importDesc.type = GFX_TEXTURE_TYPE_2D;
        importDesc.size = { viewConfigurationView.recommendedImageRectWidth, viewConfigurationView.recommendedImageRectHeight, 1 };
        importDesc.arrayLayerCount = viewCount;
        importDesc.mipLevelCount = 1;
        importDesc.sampleCount = GFX_SAMPLE_COUNT_1;
        importDesc.format = VkFormatToGfxFormat(format);
        importDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
        importDesc.currentLayout = (usageFlags & XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT : GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT;
        GFXERRCHECK(gfxDeviceImportTexture(m_gfxDevice, &importDesc, &texture));

        swapchainInfo.textures.push_back(texture);
    }
    return swapchainInfo;
}

void OpenXrRender::CreatePassthrough()
{
    if (!LoadPassthroughApi(m_instance)) {
        m_passthroughSupported = false;
        m_passthroughEnabled = false;
        return;
    }

    XrPassthroughCreateInfoFB passthroughCreateInfo{ prev::xr::open_xr::util::CreateStruct<XrPassthroughCreateInfoFB>(XR_TYPE_PASSTHROUGH_CREATE_INFO_FB) };
    passthroughCreateInfo.flags = XR_PASSTHROUGH_IS_RUNNING_AT_CREATION_BIT_FB;
    OPENXR_CHECK(xrCreatePassthroughFB(m_session, &passthroughCreateInfo, &m_passthrough), "Failed to create Passthrough.");
    if (m_passthrough == XR_NULL_HANDLE) {
        m_passthroughSupported = false;
        m_passthroughEnabled = false;
        return;
    }

    XrPassthroughLayerCreateInfoFB layerCreateInfo{ prev::xr::open_xr::util::CreateStruct<XrPassthroughLayerCreateInfoFB>(XR_TYPE_PASSTHROUGH_LAYER_CREATE_INFO_FB) };
    layerCreateInfo.passthrough = m_passthrough;
    layerCreateInfo.purpose = XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB;
    if (m_passthroughEnabled) {
        layerCreateInfo.flags = XR_PASSTHROUGH_IS_RUNNING_AT_CREATION_BIT_FB;
    }
    OPENXR_CHECK(xrCreatePassthroughLayerFB(m_session, &layerCreateInfo, &m_passthroughLayer), "Failed to create Passthrough Layer.");

    LOGI("OpenXR passthrough created (%s)", m_passthroughEnabled ? "AR" : "VR");
}

void OpenXrRender::DestroyPassthrough()
{
    if (m_passthroughLayer != XR_NULL_HANDLE) {
        OPENXR_CHECK(xrDestroyPassthroughLayerFB(m_passthroughLayer), "Failed to destroy Passthrough Layer.");
        m_passthroughLayer = XR_NULL_HANDLE;
    }
    if (m_passthrough != XR_NULL_HANDLE) {
        OPENXR_CHECK(xrDestroyPassthroughFB(m_passthrough), "Failed to destroy Passthrough.");
        m_passthrough = XR_NULL_HANDLE;
    }
}

void OpenXrRender::SetPassthroughEnabled(bool enabled)
{
    m_passthroughEnabled = enabled && m_passthroughSupported;
    if (m_passthroughLayer == XR_NULL_HANDLE) {
        return; // no session yet - applied when CreatePassthrough runs
    }
    if (m_passthroughEnabled) {
        OPENXR_CHECK(xrPassthroughLayerResumeFB(m_passthroughLayer), "Failed to resume Passthrough Layer.");
    } else {
        OPENXR_CHECK(xrPassthroughLayerPauseFB(m_passthroughLayer), "Failed to pause Passthrough Layer.");
    }
    LOGI("OpenXR passthrough %s", m_passthroughEnabled ? "enabled (AR)" : "disabled (VR)");

    // Broadcast the actual (clamped) mode so clients can react (e.g. suppress the opaque environment in AR).
    prev::event::EventChannel::Post(XrPassthroughChangedEvent{ m_passthroughEnabled });
}

void OpenXrRender::DestroySwapchain(SwapchainInfo& swapchainInfo)
{
    for (auto& texture : swapchainInfo.textures) {
        if (texture) {
            gfxTextureDestroy(texture);
        }
    }
    OPENXR_CHECK(xrDestroySwapchain(swapchainInfo.swapchain), "Failed to destroy Swapchain");
    swapchainInfo = {};
}
} // namespace prev::xr::open_xr::render

#endif