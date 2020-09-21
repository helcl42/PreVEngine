#include "TerrainShadowsShader.h"
#include "../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::shadow::shader {
TerrainShadowsShader::TerrainShadowsShader(const VkDevice device)
    : Shader(device)
{
}

void TerrainShadowsShader::InitVertexInputs()
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

void TerrainShadowsShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
}

void TerrainShadowsShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::shadow::shader