#include "FonttShader.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::font::shader {
FonttShader::FonttShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> FonttShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> FonttShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> FonttShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },

        // fragment shader
        { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        { "textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> FonttShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::font::shader