#include "CloudsShader.h"

namespace prev_test::render::renderer::sky::shader {
CloudsShader::CloudsShader(const VkDevice device)
    : Shader(device)
{
}

std::vector<VkVertexInputBindingDescription> CloudsShader::CreateVertexInputBindingDescriptors() const
{
    return {};
}

std::vector<VkVertexInputAttributeDescription> CloudsShader::CreateInputAttributeDescriptors() const
{
    return {};
}

std::vector<prev::render::shader::Shader::DescriptorSet> CloudsShader::CreateDescriptorSets() const
{
    return {
        { "uboCS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "outWeatherTexture", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> CloudsShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::sky::shader
