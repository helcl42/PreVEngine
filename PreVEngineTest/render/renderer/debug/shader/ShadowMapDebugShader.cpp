#include "ShadowMapDebugShader.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug::shader {
ShadowMapDebugShader::ShadowMapDebugShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> ShadowMapDebugShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> ShadowMapDebugShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> ShadowMapDebugShader::CreateDescriptorSets() const
{
    return {
        { "depthSampler", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> ShadowMapDebugShader::CreatePushConstantBlocks() const
{
    return {
        { VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 3 }
    };
}
} // namespace prev_test::render::renderer::debug::shader
