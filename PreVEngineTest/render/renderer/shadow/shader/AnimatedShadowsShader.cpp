#include "AnimatedShadowsShader.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::shadow::shader {
AnimatedShadowsShader::AnimatedShadowsShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> AnimatedShadowsShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> AnimatedShadowsShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> AnimatedShadowsShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> AnimatedShadowsShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::shadow::shader
