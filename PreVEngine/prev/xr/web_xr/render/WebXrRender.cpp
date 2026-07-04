#include "WebXrRender.h"

#ifdef ENABLE_WEBXR

#include <emscripten.h>
#include <webgpu/webgpu.h> // wgpuTextureRelease

#include "../../XrEvents.h"

#include "../../../common/Logger.h"
#include "../../../event/EventChannel.h"

#include <cmath>

// clang-format off
EM_JS(int, prev_webxr_preferred_color_format, (), {
    if (!navigator.gpu || !navigator.gpu.getPreferredCanvasFormat) { return 0; }
    const f = navigator.gpu.getPreferredCanvasFormat();
    if (f === 'rgba8unorm') { return 1; }
    if (f === 'bgra8unorm') { return 2; }
    return 0;
});

EM_JS(int, prev_webxr_acquire_views, (), {
    const state = Module.__prevWebXr;
    if (!state || !state.frame || !state.binding || !state.layer) {
        return 0;
    }
    const pose = state.frame.getViewerPose(state.refSpace);
    if (!pose || pose.views.length === 0) {
        return 0;
    }
    state.pose = pose;
    const sub = state.binding.getViewSubImage(state.layer, pose.views[0]);
    state.colorTexturePtr = WebGPU.importJsTexture(sub.colorTexture, state.devicePtr || 0);
    state.extentW = sub.viewport.width;
    state.extentH = sub.viewport.height;
    return pose.views.length;
});

EM_JS(int, prev_webxr_view_width, (), {
    const state = Module.__prevWebXr;
    return state ? (state.extentW | 0) : 0;
});

EM_JS(int, prev_webxr_view_height, (), {
    const state = Module.__prevWebXr;
    return state ? (state.extentH | 0) : 0;
});

EM_JS(void*, prev_webxr_color_texture, (), {
    const state = Module.__prevWebXr;
    return state ? (state.colorTexturePtr || 0) : 0;
});

// Write view i's transform (pos xyz + quat xyzw) and 16-float projection matrix into out[0..22].
EM_JS(int, prev_webxr_get_view_data, (int i, float* out), {
    const state = Module.__prevWebXr;
    if (!state || !state.pose || i >= state.pose.views.length) {
        return 0;
    }
    const v = state.pose.views[i];
    const p = v.transform.position;
    const o = v.transform.orientation;
    const m = v.projectionMatrix;
    const base = out >> 2;
    HEAPF32[base + 0] = p.x;
    HEAPF32[base + 1] = p.y;
    HEAPF32[base + 2] = p.z;
    HEAPF32[base + 3] = o.x;
    HEAPF32[base + 4] = o.y;
    HEAPF32[base + 5] = o.z;
    HEAPF32[base + 6] = o.w;
    for (let k = 0; k < 16; ++k) {
        HEAPF32[base + 7 + k] = m[k];
    }
    return 1;
});
// clang-format on

namespace {
// WebGL-convention (Y-up NDC) input - NOT interchangeable with MathUtils' Vulkan-convention extractor.
prev::util::math::Fov FovFromProjection(const float* m)
{
    prev::util::math::Fov fov{};
    fov.angleRight = std::atan((1.0f + m[8]) / m[0]);
    fov.angleLeft = std::atan((m[8] - 1.0f) / m[0]);
    fov.angleUp = std::atan((1.0f + m[9]) / m[5]);
    fov.angleDown = std::atan((m[9] - 1.0f) / m[5]);
    return fov;
}
} // namespace

namespace prev::xr::web_xr::render {
WebXrRender::WebXrRender()
{
    switch (prev_webxr_preferred_color_format()) {
    case 1:
        m_colorFormat = GFX_FORMAT_R8G8B8A8_UNORM;
        break;
    case 2:
        m_colorFormat = GFX_FORMAT_B8G8R8A8_UNORM;
        break;
    default:
        break;
    }
    LOGI("WebXr: preferred color format = %s", (m_colorFormat == GFX_FORMAT_R8G8B8A8_UNORM) ? "rgba8unorm" : "bgra8unorm");
}

WebXrRender::~WebXrRender()
{
    ReleaseFrameTextures();
}

void WebXrRender::UpdateGraphicsBinding(GfxDevice device)
{
    m_device = device;
}

GfxDevice WebXrRender::GetDevice() const
{
    return m_device;
}

void WebXrRender::OnSessionCreate()
{
    // Nothing to set up: the XRSession is created asynchronously in JS; textures are imported per frame.
}

void WebXrRender::OnSessionDestroy()
{
    ReleaseFrameTextures();
}

bool WebXrRender::BeginFrame()
{
    AcquireFrameTextures();
    PostCameraEvent();
    return !m_colorImages.empty();
}

bool WebXrRender::GetFrameImages(XrFrameImages& outImages) const
{
    if (m_colorImages.empty()) {
        return false; // no XR frame acquired yet
    }
    outImages.colorImage = m_colorImages[0];
    outImages.depthImage = {};
    outImages.imageIndex = 0;
    outImages.imagesChanged = true;
    return true;
}

uint32_t WebXrRender::GetImageCount() const
{
    return 1;
}

bool WebXrRender::HasDepthImages() const
{
    return false;
}

GfxExtent2D WebXrRender::GetExtent() const
{
    // Live size from JS - valid from session start, so dependent targets never see a transient 0x0.
    return { static_cast<uint32_t>(prev_webxr_view_width()), static_cast<uint32_t>(prev_webxr_view_height()) };
}

GfxFormat WebXrRender::GetColorFormat() const
{
    return m_colorFormat;
}

GfxFormat WebXrRender::GetDepthFormat() const
{
    return GFX_FORMAT_DEPTH32_FLOAT;
}

uint32_t WebXrRender::GetViewCount() const
{
    return m_viewCount;
}

void WebXrRender::AcquireFrameTextures()
{
    ReleaseFrameTextures();

    const int viewCount = prev_webxr_acquire_views();
    if (viewCount <= 0) {
        return;
    }
    m_viewCount = static_cast<uint32_t>(viewCount);
    m_extent = { static_cast<uint32_t>(prev_webxr_view_width()), static_cast<uint32_t>(prev_webxr_view_height()) };

    void* wgpuTexture = prev_webxr_color_texture();
    if (!wgpuTexture) {
        return;
    }

    GfxTextureImportDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_TEXTURE_IMPORT_DESCRIPTOR;
    desc.label = "WebXRColor";
    desc.nativeHandle = wgpuTexture;
    desc.type = GFX_TEXTURE_TYPE_2D;
    desc.size = { m_extent.width, m_extent.height, 1 };
    desc.arrayLayerCount = m_viewCount;
    desc.mipLevelCount = 1;
    desc.sampleCount = GFX_SAMPLE_COUNT_1;
    desc.format = m_colorFormat;
    desc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
    desc.currentLayout = GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT;

    GfxTexture texture{};
    if (gfxDeviceImportTexture(m_device, &desc, &texture) == GFX_RESULT_SUCCESS) {
        m_colorImages.push_back(texture);
    }
}

void WebXrRender::ReleaseFrameTextures()
{
    for (GfxTexture texture : m_colorImages) {
        if (!texture) {
            continue;
        }

        void* nativeHandle = nullptr;
        gfxTextureGetNativeHandle(texture, &nativeHandle);
        gfxTextureDestroy(texture);
        if (nativeHandle) {
            wgpuTextureRelease(static_cast<WGPUTexture>(nativeHandle));
        }
    }
    m_colorImages.clear();
}

void WebXrRender::PostCameraEvent()
{
    CameraEvent event{};
    const uint32_t maxEyes = static_cast<uint32_t>(MAX_VIEW_COUNT_VALUE);
    const uint32_t viewCount = (m_viewCount < maxEyes) ? m_viewCount : maxEyes;
    for (uint32_t i = 0; i < viewCount; ++i) {
        float data[23] = {}; // [0..2] position, [3..6] orientation xyzw, [7..22] projection (column-major)
        if (!prev_webxr_get_view_data(static_cast<int>(i), data)) {
            continue;
        }
        event.poses[i] = prev::util::math::Pose{ glm::quat{ data[6], data[3], data[4], data[5] }, glm::vec3{ data[0], data[1], data[2] } };
        event.fovs[i] = FovFromProjection(&data[7]);
    }
    event.count = viewCount;
    prev::event::EventChannel::Post(event);
}
} // namespace prev::xr::web_xr::render

#endif
