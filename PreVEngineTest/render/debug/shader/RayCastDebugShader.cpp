#include "RayCastDebugShader.h"
#include "../../VertexLayout.h"

#include <util/VkUtils.h>

namespace prev_test::render::debug::shader {
RayCastDebugShader::RayCastDebugShader(const VkDevice device)
    : Shader(device)
{
}

void RayCastDebugShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
    };
}

void RayCastDebugShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    // geometry shader
    AddDescriptorSet("uboGS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_GEOMETRY_BIT);

    // fragment shader
    AddDescriptorSet("uboFS", 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void RayCastDebugShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::debug::shader
