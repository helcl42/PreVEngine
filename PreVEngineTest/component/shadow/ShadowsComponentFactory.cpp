#include "ShadowsComponentFactory.h"
#include "ShadowsComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/image/DepthImageBuffer.h>
#include <prev/render/pass/RenderPassBuilder.h>
#include <prev/util/VkUtils.h>

#include <memory>

namespace prev_test::component::shadow {
std::unique_ptr<IShadowsComponent> ShadowsComponentFactory::Create() const
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };
    const VkExtent2D extent{ SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };

    std::shared_ptr<prev::render::pass::RenderPass> renderPass{ CreateRenderPass(*device) };
    std::shared_ptr<prev::core::memory::image::IImageBuffer> depthBuffer{ CreateDepthBuffer(extent, CASCADES_COUNT, *allocator) };
    auto cascades{ CreateCascades(extent, CASCADES_COUNT, depthBuffer, renderPass, *device) };

    auto result{ std::make_unique<ShadowsComponent>(CASCADES_COUNT, renderPass, depthBuffer, cascades) };
    return result;
}

std::unique_ptr<prev::render::pass::RenderPass> ShadowsComponentFactory::CreateRenderPass(prev::core::device::Device& device) const
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

    prev::render::pass::RenderPassBuilder renderPassBuilder{ device };
    return renderPassBuilder
        .AddDepthAttachment(DEPTH_FORMAT)
        .AddSubpass({ 0 })
        .AddSubpassDependencies(dependencies)
        .Build();
}

std::unique_ptr<prev::core::memory::image::IImageBuffer> ShadowsComponentFactory::CreateDepthBuffer(const VkExtent2D& extent, const uint32_t cascadesCount, prev::core::memory::Allocator& allocator) const
{
    auto depthBuffer{ std::make_unique<prev::core::memory::image::DepthImageBuffer>(allocator) };
    depthBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ extent, VK_IMAGE_TYPE_2D, DEPTH_FORMAT, VK_SAMPLE_COUNT_1_BIT, 0, false, false, VK_IMAGE_VIEW_TYPE_2D_ARRAY, cascadesCount });
    depthBuffer->CreateSampler(1.0f, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, false);
    return depthBuffer;
}

std::vector<ShadowsCascade> ShadowsComponentFactory::CreateCascades(const VkExtent2D& extent, const uint32_t cascadesCount, const std::shared_ptr<prev::core::memory::image::IImageBuffer>& depthBuffer, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, prev::core::device::Device& device) const
{
    std::vector<ShadowsCascade> cascades;
    cascades.resize(cascadesCount);
    for (uint32_t i = 0; i < cascadesCount; i++) {
        auto& cascade{ cascades.at(i) };
        cascade.imageView = prev::util::vk::CreateImageView(device, depthBuffer->GetImage(), depthBuffer->GetFormat(), VK_IMAGE_VIEW_TYPE_2D_ARRAY, depthBuffer->GetMipLevels(), VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
        cascade.frameBuffer = prev::util::vk::CreateFrameBuffer(device, *renderPass, { cascade.imageView }, extent);
    }
    return cascades;
}

} // namespace prev_test::component::shadow