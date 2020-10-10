#include "CloudsPerlinWorleyNoiseShader.h"

#include "../../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky::shader {
CloudsPerlinWorleyNoiseShader::CloudsPerlinWorleyNoiseShader(const VkDevice device)
    : Shader(device)
{
}

void CloudsPerlinWorleyNoiseShader::InitVertexInputs()
{
}

void CloudsPerlinWorleyNoiseShader::InitDescriptorSets()
{
    AddDescriptorSet("outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
}

void CloudsPerlinWorleyNoiseShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::renderer::sky::shader
