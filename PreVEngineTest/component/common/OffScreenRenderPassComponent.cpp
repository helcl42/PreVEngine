#include "OffScreenRenderPassComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::common {
OffScreenRenderPassComponent::OffScreenRenderPassComponent(const uint32_t w, const uint32_t h)
    : m_width(w)
    , m_height(h)
    , m_renderPass(nullptr)
    , m_imageBuffer(nullptr)
    , m_depthBuffer(nullptr)
    , m_frameBuffer(nullptr)
{
}

void OffScreenRenderPassComponent::Init()
{
    InitRenderPass();
    InitBuffers();
}

void OffScreenRenderPassComponent::ShutDown()
{
    ShutDownBuffers();
    ShutDownRenderPass();
}

std::shared_ptr<prev::render::pass::RenderPass> OffScreenRenderPassComponent::GetRenderPass() const
{
    return m_renderPass;
}

VkExtent2D OffScreenRenderPassComponent::GetExtent() const
{
    return VkExtent2D{ m_width, m_height };
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> OffScreenRenderPassComponent::GetColorImageBuffer() const
{
    return m_imageBuffer;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> OffScreenRenderPassComponent::GetDepthImageBuffer() const
{
    return m_depthBuffer;
}

VkFramebuffer OffScreenRenderPassComponent::GetFrameBuffer() const
{
    return m_frameBuffer;
}

void OffScreenRenderPassComponent::InitBuffers()
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    m_imageBuffer = std::make_shared<prev::core::memory::image::ColorImageBuffer>(*allocator);
    m_imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, COLOR_FORMAT, VK_SAMPLE_COUNT_1_BIT, 0, false, true, VK_IMAGE_VIEW_TYPE_2D });
    m_imageBuffer->CreateSampler();

    m_depthBuffer = std::make_shared<prev::core::memory::image::DepthImageBuffer>(*allocator);
    m_depthBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, DEPTH_FORMAT, VK_SAMPLE_COUNT_1_BIT, 0, false, false, VK_IMAGE_VIEW_TYPE_2D });
    m_depthBuffer->CreateSampler(1.0f, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, false);

    m_frameBuffer = prev::util::vk::CreateFrameBuffer(*device, *m_renderPass, { m_imageBuffer->GetImageView(), m_depthBuffer->GetImageView() }, GetExtent());
}

void OffScreenRenderPassComponent::ShutDownBuffers()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

    vkDestroyFramebuffer(*device, m_frameBuffer, nullptr);

    m_depthBuffer->Destroy();
    m_imageBuffer->Destroy();
}

void OffScreenRenderPassComponent::InitRenderPass()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    std::vector<VkSubpassDependency> dependencies{ 2 };
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Setup subpass dependencies
    // These will add the implicit attachment layout transitions specified by the attachment descriptions
    // The actual usage layout is preserved through the layout specified in the attachment reference
    // Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
    // srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
    // Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)

    //// First dependency at the start of the renderpass
    // // Does the transition from final to initial layout
    // dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL; // Producer of the dependency
    // dependencies[0].dstSubpass = 0; // Consumer is our single subpass that will wait for the execution depdendency
    // dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Match our pWaitDstStageMask when we vkQueueSubmit
    // dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a loadOp stage for color attachments
    // dependencies[0].srcAccessMask = 0; // semaphore wait already does memory dependency for us
    // dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // is a loadOp CLEAR access mask for color attachments
    // dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // // Second dependency at the end the renderpass
    // // Does the transition from the initial to the final layout
    // // Technically this is the same as the implicit subpass dependency, but we are gonna state it explicitly here
    // dependencies[1].srcSubpass = 0; // Producer of the dependency is our single subpass
    // dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; // Consumer are all commands outside of the renderpass
    // dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a storeOp stage for color attachments
    // dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // Do not block any subsequent work
    // dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // is a storeOp `STORE` access mask for color attachments
    // dependencies[1].dstAccessMask = 0;
    // dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    m_renderPass = std::make_shared<prev::render::pass::RenderPass>(*device);
    m_renderPass->AddColorAttachment(COLOR_FORMAT, VK_SAMPLE_COUNT_1_BIT, { 0.5f, 0.5f, 0.5f, 1.0f }, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_renderPass->AddDepthAttachment(DEPTH_FORMAT, VK_SAMPLE_COUNT_1_BIT);
    m_renderPass->AddSubpass({ 0, 1 });
    m_renderPass->AddSubpassDependencies(dependencies);
    m_renderPass->Create();
}

void OffScreenRenderPassComponent::ShutDownRenderPass()
{
    m_renderPass->Destroy();
}
} // namespace prev_test::component::common