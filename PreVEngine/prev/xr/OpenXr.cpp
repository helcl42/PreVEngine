#include "OpenXr.h"

#ifdef ENABLE_XR

namespace prev::xr {
OpenXr::OpenXr()
    : m_core{ std::make_unique<core::OpenXrCore>() }
    , m_render{ std::make_unique<render::OpenXrRender>(m_core->GetInstance(), m_core->GetSystemId()) }
    , m_input{ std::make_unique<input::OpenXrInput>(m_core->GetInstance(), m_core->GetSystemId()) }
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

void OpenXr::UpdateGraphicsBinding(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    m_render->UpdateGraphicsBinding(instance, physicalDevice, device, queueFamilyIndex, queueIndex);
}

std::vector<std::string> OpenXr::GetVulkanInstanceExtensions() const
{
    return m_core->GetVulkanInstanceExtensions();
}

std::vector<std::string> OpenXr::GetVulkanDeviceExtensions() const
{
    return m_core->GetVulkanDeviceExtensions();
}

VkPhysicalDevice OpenXr::GetPhysicalDevice(VkInstance instance) const
{
    return m_core->GetPhysicalDevice(instance);
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

std::vector<VkImage> OpenXr::GetColorImages() const
{
    return m_render->GetColorImages();
}

std::vector<VkImageView> OpenXr::GetColorImagesViews() const
{
    return m_render->GetColorImagesViews();
}

std::vector<VkImage> OpenXr::GetDepthImages() const
{
    return m_render->GetDepthImages();
}

std::vector<VkImageView> OpenXr::GetDepthImagesViews() const
{
    return m_render->GetDepthImagesViews();
}

VkExtent2D OpenXr::GetExtent() const
{
    return m_render->GetExtent();
}

VkFormat OpenXr::GetColorFormat() const
{
    return m_render->GetColorFormat();
}

VkFormat OpenXr::GetDepthFormat() const
{
    return m_render->GetDepthFormat();
}

uint32_t OpenXr::GetViewCount() const
{
    return m_render->GetViewCount();
}

uint32_t OpenXr::GetCurrentSwapchainIndex() const
{
    return m_render->GetCurrentSwapchainIndex();
}

float OpenXr::GetCurrentDeltaTime() const
{
    return m_render->GetCurrentDeltaTime();
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
} // namespace prev::xr

#endif
