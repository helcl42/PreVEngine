#include "PerlinWorleyNoiseShader.h"

#include "../../VertexLayout.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::sky::shader {
PerlinWorleyNoiseShader::PerlinWorleyNoiseShader(const VkDevice device)
    : Shader(device)
{
}

void PerlinWorleyNoiseShader::InitVertexInputs()
{
}

void PerlinWorleyNoiseShader::InitDescriptorSets()
{
    AddDescriptorSet("outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
}

void PerlinWorleyNoiseShader::InitPushConstantsBlocks()
{
}
} // namespace prev_test::render::sky::shader