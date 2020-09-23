#include "ConeStepMappedShader.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::normal::shader {
ConeStepMappedShader::ConeStepMappedShader(const VkDevice device)
    : Shader(device)
{
}

void ConeStepMappedShader::InitVertexInputs()
{
    m_inputBindingDescriptions = {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };

    m_inputAttributeDescriptions = {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }))
    };
}

void ConeStepMappedShader::InitDescriptorSets()
{
    // vertex shader
    AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    // fragment shader
    AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void ConeStepMappedShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::normal::shader
