#include "CloudsPerlinWorleyNoiseShader.h"
#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
CloudsPerlinWorleyNoiseShader::CloudsPerlinWorleyNoiseShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> CloudsPerlinWorleyNoiseShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/clouds_perlin_worley_noise_3d_comp.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> CloudsPerlinWorleyNoiseShader::CreateVertexInputBindingDescriptors() const
{
    return {};
}

std::vector<VkVertexInputAttributeDescription> CloudsPerlinWorleyNoiseShader::CreateInputAttributeDescriptors() const
{
    return {};
}

std::vector<prev::render::shader::Shader::DescriptorSet> CloudsPerlinWorleyNoiseShader::CreateDescriptorSets() const
{
    return {
        { "outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> CloudsPerlinWorleyNoiseShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::sky::shader
