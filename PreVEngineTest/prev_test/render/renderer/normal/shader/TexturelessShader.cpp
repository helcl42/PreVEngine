#include "TexturelessShader.h"

#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::normal::shader {
TexturelessShader::TexturelessShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> TexturelessShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/normal/textureless_vert.spv") },
        { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/normal/textureless_frag.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> TexturelessShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> TexturelessShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> TexturelessShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },

        // fragment shader
        { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        { "depthSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> TexturelessShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::normal::shader
