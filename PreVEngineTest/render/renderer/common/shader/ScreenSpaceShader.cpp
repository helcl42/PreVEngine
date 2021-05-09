#include "ScreenSpaceShader.h"
#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::common::shader {
ScreenSpaceShader::ScreenSpaceShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> ScreenSpaceShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/common/screen_space_vert.spv") },
        { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/common/screen_space_frag.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> ScreenSpaceShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX),
    };
}

std::vector<VkVertexInputAttributeDescription> ScreenSpaceShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> ScreenSpaceShader::CreateDescriptorSets() const
{
    return {
        { "image", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> ScreenSpaceShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::common::shader
