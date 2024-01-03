#include "OffScreenRenderPassComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
#include <prev/render/pass/RenderPassBuilder.h>
#include <prev/util/VkUtils.h>

#include <stdexcept>

namespace prev_test::component::common {
OffScreenRenderPassComponent::OffScreenRenderPassComponent(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats)
    : m_extent{ extent }
    , m_depthFormat{ depthFormat }
    , m_colorFormats{ colorFormats }
    , m_renderPass{}
    , m_depthBuffer{}
    , m_depthSampler{}
    , m_colorBuffers{}
    , m_colorSamplers{}
    , m_frameBuffer{}
{
}

void OffScreenRenderPassComponent::Init()
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    // create render pass
    const uint32_t depthDependenciesOffset{ (m_depthFormat != VK_FORMAT_UNDEFINED) ? 2u : 0u };
    const uint32_t allDependenciesCount{ depthDependenciesOffset + 2u * static_cast<uint32_t>(m_colorFormats.size()) };

    std::vector<VkSubpassDependency> dependencies(allDependenciesCount);
    if (m_depthFormat != VK_FORMAT_UNDEFINED) {
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }

    for (uint32_t index = depthDependenciesOffset; index < allDependenciesCount; index += 2) {
        dependencies[index + 0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[index + 0].dstSubpass = 0;
        dependencies[index + 0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[index + 0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[index + 0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[index + 0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[index + 0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[index + 1].srcSubpass = 0;
        dependencies[index + 1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[index + 1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[index + 1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[index + 1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[index + 1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[index + 1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }

    std::vector<uint32_t> attachmentIndices;
    uint32_t attachmentIndex{ 0 };

    prev::render::pass::RenderPassBuilder renderPassBuilder{ *device };
    if (m_depthFormat != VK_FORMAT_UNDEFINED) {
        renderPassBuilder.AddDepthAttachment(m_depthFormat, VK_SAMPLE_COUNT_1_BIT, { MAX_DEPTH, 0 });
        attachmentIndices.push_back(attachmentIndex);
        ++attachmentIndex;
    }
    for (size_t i = 0; i < m_colorFormats.size(); ++i) {
        const auto colorFormat{ m_colorFormats[i] };
        renderPassBuilder.AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, { { 0.5f, 0.5f, 0.5f, 1.0f } }, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        attachmentIndices.push_back(attachmentIndex);
        ++attachmentIndex;
    }
    m_renderPass = renderPassBuilder
                       .AddSubpass(attachmentIndices)
                       .AddSubpassDependencies(dependencies)
                       .Build();

    prev::render::buffer::image::ImageBufferFactory imageBufferFactory{};

    // create image buffers and corresponding samplers
    if (m_depthFormat != VK_FORMAT_UNDEFINED) {
        m_depthBuffer = imageBufferFactory.CreateDepth(prev::render::buffer::image::ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, m_depthFormat, VK_SAMPLE_COUNT_1_BIT, 0, false, VK_IMAGE_VIEW_TYPE_2D }, *allocator);
        m_depthSampler = std::make_shared<prev::render::sampler::Sampler>(*device, 1.0f, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    }

    for (uint32_t i = 0; i < m_colorFormats.size(); ++i) {
        const auto colorFormat{ m_colorFormats[i] };

        auto colorImageBuffer{ imageBufferFactory.CreateColor(prev::render::buffer::image::ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, colorFormat, VK_SAMPLE_COUNT_1_BIT, 0, false, VK_IMAGE_VIEW_TYPE_2D }, *allocator) };
        auto colorImageBufferSampler{ std::make_shared<prev::render::sampler::Sampler>(*device, static_cast<float>(colorImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };

        m_colorBuffers.emplace_back(std::move(colorImageBuffer));
        m_colorSamplers.emplace_back(std::move(colorImageBufferSampler));
    }

    std::vector<VkImageView> imageViews;
    if (m_depthFormat != VK_FORMAT_UNDEFINED) {
        imageViews.push_back(m_depthBuffer->GetImageView());
    }
    for (const auto& colorBuffer : m_colorBuffers) {
        imageViews.push_back(colorBuffer->GetImageView());
    }

    m_frameBuffer = prev::util::vk::CreateFrameBuffer(*device, *m_renderPass, imageViews, GetExtent());
}

void OffScreenRenderPassComponent::ShutDown()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

    vkDestroyFramebuffer(*device, m_frameBuffer, nullptr);
    m_frameBuffer = nullptr;

    m_depthSampler = nullptr;
    m_depthBuffer = nullptr;

    m_colorSamplers.clear();
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

std::shared_ptr<prev::render::buffer::image::IImageBuffer> OffScreenRenderPassComponent::GetColorImageBuffer(const uint32_t index) const
{
    if (index >= static_cast<uint32_t>(m_colorBuffers.size())) {
        throw std::runtime_error("Invalid color buffer index used: " + std::to_string(index));
    }
    return m_colorBuffers[index];
}

std::shared_ptr<prev::render::sampler::Sampler> OffScreenRenderPassComponent::GetColorSampler(const uint32_t index) const
{
    if (index >= static_cast<uint32_t>(m_colorSamplers.size())) {
        throw std::runtime_error("Invalid color buffer sampler index used: " + std::to_string(index));
    }
    return m_colorSamplers[index];
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> OffScreenRenderPassComponent::GetDepthImageBuffer() const
{
    if (!m_depthBuffer) {
        throw std::runtime_error("Invalid depth buffer.");
    }
    return m_depthBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> OffScreenRenderPassComponent::GetDepthSampler() const
{
    if (!m_depthSampler) {
        throw std::runtime_error("Invalid depth sampler.");
    }
    return m_depthSampler;
}

VkFramebuffer OffScreenRenderPassComponent::GetFrameBuffer() const
{
    return m_frameBuffer;
}
} // namespace prev_test::component::common