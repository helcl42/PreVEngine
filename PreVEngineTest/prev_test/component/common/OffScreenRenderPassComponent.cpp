#include "OffScreenRenderPassComponent.h"

#include <stdexcept>

namespace prev_test::component::common {
OffScreenRenderPassComponent::OffScreenRenderPassComponent(prev::core::device::Device& device, const VkExtent2D& extent, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::ImageBuffer>& depthBuffer, const std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>& colorBuffers, const VkFramebuffer frameBuffer)
    : m_device{ device }
    , m_extent{ extent }
    , m_renderPass{ renderPass }
    , m_depthBuffer{ depthBuffer }
    , m_colorBuffers{ colorBuffers }
    , m_frameBuffer{ frameBuffer }
{
}

OffScreenRenderPassComponent::~OffScreenRenderPassComponent()
{
    m_device.WaitIdle();

    vkDestroyFramebuffer(m_device, m_frameBuffer, nullptr);
    m_frameBuffer = nullptr;

    m_depthBuffer = nullptr;

    m_colorBuffers.clear();

    m_renderPass = nullptr;
}

std::shared_ptr<prev::render::pass::RenderPass> OffScreenRenderPassComponent::GetRenderPass() const
{
    return m_renderPass;
}

const VkExtent2D& OffScreenRenderPassComponent::GetExtent() const
{
    return m_extent;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> OffScreenRenderPassComponent::GetColorImageBuffer(const uint32_t index) const
{
    if (index >= static_cast<uint32_t>(m_colorBuffers.size())) {
        throw std::runtime_error("Invalid color buffer index used: " + std::to_string(index));
    }
    return m_colorBuffers[index];
}

std::shared_ptr<prev::render::buffer::ImageBuffer> OffScreenRenderPassComponent::GetDepthImageBuffer() const
{
    if (!m_depthBuffer) {
        throw std::runtime_error("Invalid depth buffer.");
    }
    return m_depthBuffer;
}

VkFramebuffer OffScreenRenderPassComponent::GetFrameBuffer() const
{
    return m_frameBuffer;
}
} // namespace prev_test::component::common