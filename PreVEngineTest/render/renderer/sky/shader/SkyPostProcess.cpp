#include "SkyPostProcess.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
SkyPostProcessShader::SkyPostProcessShader(const VkDevice device)
    : Shader(device)
{
}

void SkyPostProcessShader::InitVertexInputs()
{
}

void SkyPostProcessShader::InitDescriptorSets()
{
    AddDescriptorSet("outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    AddDescriptorSet("skyTex", 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("bloomTex", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    AddDescriptorSet("uboCS", 3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
}

void SkyPostProcessShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::sky::shader
