#include "CloudsShader.h"

namespace prev_test::render::sky::shader {
CloudsShader::CloudsShader(const VkDevice device)
    : Shader(device)
{
}

void CloudsShader::InitVertexInputs()
{
}

void CloudsShader::InitDescriptorSets()
{
    AddDescriptorSet("uboCS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    AddDescriptorSet("outWeatherTexture", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
}

void CloudsShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::sky::shader