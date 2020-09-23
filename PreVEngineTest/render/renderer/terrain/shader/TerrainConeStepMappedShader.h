#ifndef __TERRAIN_CONE_STEP_MAPPED_SHADER_H__
#define __TERRAIN_CONE_STEP_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::terrain::shader {
class TerrainConeStepMappedShader final : public prev::render::shader::Shader {
public:
    TerrainConeStepMappedShader(const VkDevice device);

    ~TerrainConeStepMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::terrain::shader

#endif