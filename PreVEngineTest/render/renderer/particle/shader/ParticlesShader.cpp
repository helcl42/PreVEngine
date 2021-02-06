#include "ParticlesShader.h"
#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::particle::shader {
ParticlesShader::ParticlesShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> ParticlesShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/particle/particles_vert.spv") },
        { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/particle/particles_frag.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> ParticlesShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX),
        prev::util::VkUtils::CreateVertexInputBindingDescription(1, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT }), VK_VERTEX_INPUT_RATE_INSTANCE)
    };
}

std::vector<VkVertexInputAttributeDescription> ParticlesShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),

        // mat4 -> transformation matrix
        // vec4 -> texture offset curr, next stages
        // float -> blend factor of curr, next texture stages
        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 5, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 6, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),

        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 7, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),
        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 8, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2 })),

        prev::util::VkUtils::CreateVertexInputAttributeDescription(1, 9, VK_FORMAT_R32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> ParticlesShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },

        // fragment shader
        { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> ParticlesShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::particle::shader
