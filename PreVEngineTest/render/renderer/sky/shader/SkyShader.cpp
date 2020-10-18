#include "SkyShader.h"

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
SkyShader::SkyShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> SkyShader::CreateVertexInputBindingDescriptors() const
{
    return {};
}

std::vector<VkVertexInputAttributeDescription> SkyShader::CreateInputAttributeDescriptors() const
{
    return {};
}

std::vector<prev::render::shader::Shader::DescriptorSet> SkyShader::CreateDescriptorSets() const
{
    return {
        { "outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "outBloom", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "outAlphaness", 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "outCloudDistance", 3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },

        { "perlinNoiseTex", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "weatherTex", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },

        { "uboCS", 6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> SkyShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::sky::shader
