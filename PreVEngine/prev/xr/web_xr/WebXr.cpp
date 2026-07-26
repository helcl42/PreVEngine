#include "WebXr.h"

#ifdef ENABLE_WEBXR

namespace prev::xr::web_xr {
WebXr::WebXr(prev::core::engine::XrMode /*xrMode*/, bool /*colorManaged*/) // web mode is chosen at runtime via the Enter VR / Enter AR buttons
    : m_core{ std::make_unique<core::WebXrCore>() }
    , m_render{ std::make_unique<render::WebXrRender>() }
    , m_input{ std::make_unique<input::WebXrInput>() }
{
}

std::vector<std::string> WebXr::GetRequiredInstanceExtensions() const
{
    return {};
}

std::vector<std::string> WebXr::GetRequiredDeviceExtensions() const
{
    return {};
}

GfxAdapter WebXr::GetAdapter(GfxInstance /*instance*/) const
{
    return {}; // null => engine picks its default adapter
}

void WebXr::UpdateGraphicsBinding(GfxInstance /*instance*/, GfxAdapter /*adapter*/, GfxDevice device, GfxQueue /*queue*/)
{
    m_render->UpdateGraphicsBinding(device);
}

void WebXr::CreateSession()
{
    m_core->CreateSession(m_render->GetDevice());
    m_render->OnSessionCreate();
}

void WebXr::DestroySession()
{
    m_render->OnSessionDestroy();
    m_core->DestroySession();
}

void WebXr::PollEvents()
{
}

void WebXr::PollActions()
{
    m_input->PollActions();
}

bool WebXr::BeginFrame()
{
    if (!m_core->IsSessionRunning()) {
        return false;
    }
    return m_render->BeginFrame();
}

bool WebXr::EndFrame()
{
    return m_core->IsSessionRunning();
}

bool WebXr::GetFrameImages(XrFrameImages& outImages) const
{
    return m_render->GetFrameImages(outImages);
}

uint32_t WebXr::GetImageCount() const
{
    return m_render->GetImageCount();
}

bool WebXr::HasDepthImages() const
{
    return m_render->HasDepthImages();
}

GfxExtent2D WebXr::GetExtent() const
{
    return m_render->GetExtent();
}

GfxFormat WebXr::GetColorFormat() const
{
    return m_render->GetColorFormat();
}

GfxFormat WebXr::GetDepthFormat() const
{
    return m_render->GetDepthFormat();
}

uint32_t WebXr::GetViewCount() const
{
    return m_render->GetViewCount();
}

float WebXr::GetCurrentDeltaTime() const
{
    return m_core->GetCurrentDeltaTime();
}

void WebXr::RunFrameLoop(const std::function<bool()>& tick)
{
    m_core->RunFrameLoop(tick);
}
} // namespace prev::xr::web_xr

#endif
