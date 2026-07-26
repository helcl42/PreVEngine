#include "OpenXr.h"

#ifdef ENABLE_OPENXR

#include "OpenXrLoader.h"

namespace prev::xr::open_xr {
OpenXr::OpenXr(prev::core::engine::XrMode xrMode, bool colorManaged)
    : m_loaded{ OpenXrLoader::Instance().IsLoaded() }
    , m_core{ std::make_unique<core::OpenXrCore>() }
    , m_render{ std::make_unique<render::OpenXrRender>(m_core->GetInstance(), m_core->GetSystemId(), m_core->IsPassthroughSupported(), xrMode == prev::core::engine::XrMode::AR, colorManaged) }
    , m_input{ std::make_unique<input::OpenXrInput>(m_core->GetInstance(), m_core->GetSystemId(), m_core->IsHandTrackingSupported()) }
{
    m_core->RegisterOpenXrEventObserver(*m_render);
    m_core->RegisterOpenXrEventObserver(*m_input);
}

OpenXr::~OpenXr()
{
    m_core->UnregisterOpenXrEventObserver(*m_input);
    m_core->UnregisterOpenXrEventObserver(*m_render);

    m_input = {};
    m_render = {};
    m_core = {};
}

void OpenXr::PollEvents()
{
    m_core->PollEvents();
}

void OpenXr::UpdateGraphicsBinding(GfxInstance instance, GfxAdapter adapter, GfxDevice device, GfxQueue queue)
{
    GfxQueueInfo queueInfo{};
    gfxQueueGetInfo(queue, &queueInfo);

    m_render->UpdateGraphicsBinding(instance, adapter, device, queueInfo.queueFamilyIndex, queueInfo.queueIndex);
}

std::vector<std::string> OpenXr::GetRequiredInstanceExtensions() const
{
    return m_core->GetVulkanInstanceExtensions();
}

std::vector<std::string> OpenXr::GetRequiredDeviceExtensions() const
{
    return m_core->GetVulkanDeviceExtensions();
}

GfxAdapter OpenXr::GetAdapter(GfxInstance instance) const
{
    return m_core->GetAdapter(instance);
}

void OpenXr::CreateSession()
{
    m_core->CreateSession(m_render->GetGraphicsBinding(), m_render->GetViewConfiguration());

    m_render->OnSessionCreate(m_core->GetSession());
    m_input->OnSessionCreate(m_core->GetSession());

    m_render->OnReferenceSpaceCreate(m_core->GetReferenceSpace());
    m_input->OnReferenceSpaceCreate(m_core->GetReferenceSpace());
}

void OpenXr::DestroySession()
{
    m_input->OnReferenceSpaceDestroy();
    m_render->OnReferenceSpaceDestroy();

    m_input->OnSessionDestroy();
    m_render->OnSessionDestroy();

    m_core->DestroySession();
}

bool OpenXr::GetFrameImages(XrFrameImages& outImages) const
{
    if (m_render->GetImageCount() == 0) {
        return false; // no session/swapchain yet
    }
    const uint32_t index{ m_render->GetCurrentSwapchainIndex() };
    outImages.colorImage = m_render->GetColorImage(index);
    outImages.depthImage = m_render->GetDepthImage(index);
    outImages.imageIndex = index;
    outImages.imagesChanged = false;
    return true;
}

uint32_t OpenXr::GetImageCount() const
{
    return m_render->GetImageCount();
}

bool OpenXr::HasDepthImages() const
{
    return m_render->HasDepthImages();
}

GfxExtent2D OpenXr::GetExtent() const
{
    return m_render->GetExtent();
}

GfxFormat OpenXr::GetColorFormat() const
{
    return m_render->GetColorFormat();
}

GfxFormat OpenXr::GetDepthFormat() const
{
    return m_render->GetDepthFormat();
}

uint32_t OpenXr::GetViewCount() const
{
    return m_render->GetViewCount();
}

float OpenXr::GetCurrentDeltaTime() const
{
    return m_render->GetCurrentDeltaTime();
}

void OpenXr::RunFrameLoop(const std::function<bool()>& tick)
{
    // Frame pacing comes from the runtime (xrWaitFrame in BeginFrame).
    while (tick()) {
    }
}

bool OpenXr::BeginFrame()
{
    if (!m_core->IsSessionRunning()) {
        return false;
    }
    return m_render->BeginFrame();
}

bool OpenXr::EndFrame()
{
    if (!m_core->IsSessionRunning()) {
        return false;
    }
    return m_render->EndFrame();
}

void OpenXr::PollActions()
{
    m_input->PollActions(m_render->GetCurrentTime());
}
} // namespace prev::xr::open_xr

#endif
