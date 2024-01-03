#include "SkyCompositeShader.h"

#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
SkyCompositeShader::SkyCompositeShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> SkyCompositeShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_composite_vert.spv") },
        { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_composite_frag.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> SkyCompositeShader::CreateVertexInputBindingDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
    };
}

std::vector<VkVertexInputAttributeDescription> SkyCompositeShader::CreateInputAttributeDescriptors() const
{
    return {
        prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
        prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
    };
}

std::vector<prev::render::shader::Shader::DescriptorSet> SkyCompositeShader::CreateDescriptorSets() const
{
    return {
        // fragment shader
        { "colorTex", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        { "depthTex", 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> SkyCompositeShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::sky::shader
