#include "TextureDebugShader.h"
#include "../../VertexLayout.h"

#include <util/VkUtils.h>

namespace prev_test::render::debug::shader {
TextureDebugShader::TextureDebugShader(const VkDevice device)
    : Shader(device)
{
}

void TextureDebugShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
    };
}

void TextureDebugShader::InitDescriptorSets()
{
    AddDescriptorSet("imageSampler", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void TextureDebugShader::InitPushConstantsBlocks()
{
    AddPushConstantBlock(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 3);
}
} // namespace prev_test::render::debug::shader