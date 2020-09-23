#include "SunOcclusionShader.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
SunOcclusionShader::SunOcclusionShader(const VkDevice device)
    : Shader(device)
{
}

void SunOcclusionShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0)
    };
}

void SunOcclusionShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
}

void SunOcclusionShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::sky::shader
