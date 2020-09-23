#include "BoundingVolumeDebugShader.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug::shader {
BoundingVolumeDebugShader::BoundingVolumeDebugShader(const VkDevice device)
    : Shader(device)
{
}

void BoundingVolumeDebugShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
    };
}

void BoundingVolumeDebugShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    // fragment shader
    AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void BoundingVolumeDebugShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::debug::shader