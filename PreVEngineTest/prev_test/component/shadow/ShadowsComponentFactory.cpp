#include "ShadowsComponentFactory.h"
#include "ShadowsComponent.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pass/RenderPassBuilder.h>
#include <prev/util/VkUtils.h>

#include <memory>

namespace prev_test::component::shadow {
ShadowsComponentFactory::ShadowsComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<IShadowsComponent> ShadowsComponentFactory::Create() const
{
    const VkExtent2D extent{ SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };

    std::shared_ptr<prev::render::pass::RenderPass> renderPass{ CreateRenderPass() };
    std::shared_ptr<prev::render::buffer::ImageBuffer> depthBuffer{ CreateDepthBuffer(extent, CASCADES_COUNT) };
    std::shared_ptr<prev::render::sampler::Sampler> depthSampler{ CreateSampler(static_cast<float>(depthBuffer->GetMipLevels())) };
    auto cascades{ CreateCascades(extent, CASCADES_COUNT, *depthBuffer, *renderPass) };

    auto result{ std::make_unique<ShadowsComponent>(m_device, CASCADES_COUNT, renderPass, depthBuffer, depthSampler, cascades) };
    return result;
}

std::unique_ptr<prev::render::pass::RenderPass> ShadowsComponentFactory::CreateRenderPass() const
{
    std::vector<VkSubpassDependency> dependencies{ 2 };
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

    prev::render::pass::RenderPassBuilder renderPassBuilder{ m_device };
    return renderPassBuilder
        .AddDepthAttachment(DEPTH_FORMAT, VK_SAMPLE_COUNT_1_BIT, { MAX_DEPTH, 0 })
        .AddSubpass({ 0 })
        .AddSubpassDependencies(dependencies)
        .Build();
}

std::unique_ptr<prev::render::buffer::ImageBuffer> ShadowsComponentFactory::CreateDepthBuffer(const VkExtent2D& extent, const uint32_t cascadesCount) const
{
    auto depthBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                           .SetExtent({ extent.width, extent.height, 1 })
                           .SetFormat(DEPTH_FORMAT)
                           .SetType(VK_IMAGE_TYPE_2D)
                           .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                           .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                           .SetViewType(VK_IMAGE_VIEW_TYPE_2D_ARRAY)
                           .SetLayerCount(cascadesCount)
                           .Build();
    return depthBuffer;
}

std::unique_ptr<prev::render::sampler::Sampler> ShadowsComponentFactory::CreateSampler(const float maxLod) const
{
    return std::make_unique<prev::render::sampler::Sampler>(m_device, maxLod, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST);
}

std::vector<ShadowsCascade> ShadowsComponentFactory::CreateCascades(const VkExtent2D& extent, const uint32_t cascadesCount, const prev::render::buffer::ImageBuffer& depthBuffer, const prev::render::pass::RenderPass& renderPass) const
{
    std::vector<ShadowsCascade> cascades;
    cascades.resize(cascadesCount);
    for (uint32_t i = 0; i < cascadesCount; ++i) {
        auto& cascade{ cascades[i] };
        cascade.imageView = prev::util::vk::CreateImageView(m_device, depthBuffer.GetImage(), depthBuffer.GetFormat(), VK_IMAGE_VIEW_TYPE_2D_ARRAY, depthBuffer.GetMipLevels(), VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
        cascade.frameBuffer = prev::util::vk::CreateFrameBuffer(m_device, renderPass, { cascade.imageView }, extent);
    }
    return cascades;
}

} // namespace prev_test::component::shadow