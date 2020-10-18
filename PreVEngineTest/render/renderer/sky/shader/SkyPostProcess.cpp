#include "SkyPostProcess.h"
#include "../../../../common/AssetManager.h"
#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
SkyPostProcessShader::SkyPostProcessShader(const VkDevice device)
    : Shader(device)
{
}

std::map<VkShaderStageFlagBits, std::string> SkyPostProcessShader::GetPaths()
{
    return {
        { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sky_post_process_comp.spv") }
    };
}

std::vector<VkVertexInputBindingDescription> SkyPostProcessShader::CreateVertexInputBindingDescriptors() const
{
    return {};
}

std::vector<VkVertexInputAttributeDescription> SkyPostProcessShader::CreateInputAttributeDescriptors() const
{
    return {};
}

std::vector<prev::render::shader::Shader::DescriptorSet> SkyPostProcessShader::CreateDescriptorSets() const
{
    return {
        { "outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },

        { "skyTex", 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { "bloomTex", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT },

        { "uboCS", 3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT }
    };
}

std::vector<prev::render::shader::Shader::PushConstantBlock> SkyPostProcessShader::CreatePushConstantBlocks() const
{
    return {};
}
} // namespace prev_test::render::renderer::sky::shader
