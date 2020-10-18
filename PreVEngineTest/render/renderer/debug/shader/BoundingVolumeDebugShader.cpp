#include "BoundingVolumeDebugShader.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug::shader {
BoundingVolumeDebugShader::BoundingVolumeDebugShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> BoundingVolumeDebugShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> BoundingVolumeDebugShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> BoundingVolumeDebugShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },

        // fragment shader
        { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> BoundingVolumeDebugShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::debug::shader