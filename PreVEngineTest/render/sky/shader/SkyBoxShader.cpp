#include "SkyBoxShader.h"

#include "../../VertexLayout.h"

#include <util/VkUtils.h>

namespace prev_test::render::sky::shader {
SkyBoxShader::SkyBoxShader(const VkDevice device)
    : Shader(device)
{
}

void SkyBoxShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
    };
}

void SkyBoxShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    // fragment shader
    AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddDescriptorSet("cubeMap1", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void SkyBoxShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::sky::shader
