#include "OffScreenRenderPassComponentFactory.h"

#include "OffScreenRenderPassComponent.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pass/RenderPassBuilder.h>
#include <prev/util/VkUtils.h>

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pass/RenderPassBuilder.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::common {

constexpr bool ANISOTROPIC_FILTERING_ENABLED{ true };
constexpr float MAX_ANISOTROPY_LEVEL{ 4.0f };

OffScreenRenderPassComponentFactory::OffScreenRenderPassComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<IOffScreenRenderPassComponent> OffScreenRenderPassComponentFactory::Create(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats, const uint32_t viewCount) const
{
    // create render pass
    const uint32_t depthDependenciesOffset{ (depthFormat != VK_FORMAT_UNDEFINED) ? 2u : 0u };
    const uint32_t allDependenciesCount{ depthDependenciesOffset + 2u * static_cast<uint32_t>(colorFormats.size()) };

    std::vector<VkSubpassDependency> dependencies(allDependenciesCount);
    if (depthFormat != VK_FORMAT_UNDEFINED) {
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

    prev::render::pass::RenderPassBuilder renderPassBuilder{ m_device };
    if (depthFormat != VK_FORMAT_UNDEFINED) {
        renderPassBuilder.AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, { MAX_DEPTH, 0 }, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        attachmentIndices.push_back(attachmentIndex);
        ++attachmentIndex;
    }
    for (size_t i = 0; i < colorFormats.size(); ++i) {
        const auto colorFormat{ colorFormats[i] };
        renderPassBuilder.AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, { { 0.5f, 0.5f, 0.5f, 1.0f } }, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        attachmentIndices.push_back(attachmentIndex);
        ++attachmentIndex;
    }

    std::shared_ptr<prev::render::pass::RenderPass> renderPass = renderPassBuilder
                                                                     .AddSubpass(attachmentIndices)
                                                                     .AddSubpassDependencies(dependencies)
                                                                     .Build();

    std::shared_ptr<prev::render::buffer::ImageBuffer> depthBuffer{};
    std::shared_ptr<prev::render::sampler::Sampler> depthSampler{};
    // create image buffers and corresponding samplers
    if (depthFormat != VK_FORMAT_UNDEFINED) {
        depthBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                          .SetExtent({ extent.width, extent.height, 1 })
                          .SetFormat(depthFormat)
                          .SetType(VK_IMAGE_TYPE_2D)
                          .SetViewType(prev::util::vk::GetImageViewType(viewCount))
                          .SetLayerCount(viewCount)
                          .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                          .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
                          .Build();
        depthSampler = std::make_shared<prev::render::sampler::Sampler>(m_device, 1.0f, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    }

    std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> colorBuffers{};
    std::vector<std::shared_ptr<prev::render::sampler::Sampler>> colorSamplers{};
    for (uint32_t i = 0; i < colorFormats.size(); ++i) {
        const auto colorFormat{ colorFormats[i] };
        auto colorImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                    .SetExtent({ extent.width, extent.height, 1 })
                                    .SetFormat(colorFormat)
                                    .SetType(VK_IMAGE_TYPE_2D)
                                    .SetViewType(prev::util::vk::GetImageViewType(viewCount))
                                    .SetLayerCount(viewCount)
                                    .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                                    .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                                    .Build();
        auto colorImageBufferSampler{ std::make_shared<prev::render::sampler::Sampler>(m_device, static_cast<float>(colorImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, ANISOTROPIC_FILTERING_ENABLED, MAX_ANISOTROPY_LEVEL) };

        colorBuffers.emplace_back(std::move(colorImageBuffer));
        colorSamplers.emplace_back(std::move(colorImageBufferSampler));
    }

    std::vector<VkImageView> imageViews;
    if (depthFormat != VK_FORMAT_UNDEFINED) {
        imageViews.push_back(depthBuffer->GetImageView());
    }
    for (const auto& colorBuffer : colorBuffers) {
        imageViews.push_back(colorBuffer->GetImageView());
    }

    auto frameBuffer = prev::util::vk::CreateFrameBuffer(m_device, *renderPass, imageViews, extent);

    return std::make_unique<OffScreenRenderPassComponent>(m_device, extent, renderPass, depthBuffer, depthSampler, colorBuffers, colorSamplers, frameBuffer);
}
} // namespace prev_test::component::common