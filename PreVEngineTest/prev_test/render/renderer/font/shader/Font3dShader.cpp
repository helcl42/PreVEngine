#include "Font3dShader.h"

#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::font::shader {
Font3dShader::Font3dShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> Font3dShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/font/font_3d_vert.spv") },
        { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/font/font_frag.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> Font3dShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> Font3dShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> Font3dShader::CreateDescriptorSets() const
{
    return {
        // vertex shader
        { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },

        // fragment shader
        { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        { "alphaSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> Font3dShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::font::shader