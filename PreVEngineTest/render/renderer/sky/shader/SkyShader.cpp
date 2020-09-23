#include "SkyShader.h"

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
SkyShader::SkyShader(const VkDevice device)
    : Shader(device)
{
}

void SkyShader::InitVertexInputs()
{
}

void SkyShader::InitDescriptorSets()
{
    AddDescriptorSet("outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("outBloom", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("outAlphaness", 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("outCloudDistance", 3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    AddDescriptorSet("perlinNoiseTex", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("weatherTex", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    AddDescriptorSet("uboCS", 6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
}

void SkyShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::sky::shader
