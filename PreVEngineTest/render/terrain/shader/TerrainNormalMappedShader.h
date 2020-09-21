#ifndef __TERRAIN_NORMAL_SHADER_H__
#define __TERRAIN_NORMAL_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::terrain::shader {
class TerrainNormalMappedShader final : public prev::render::shader::Shader {
public:
    TerrainNormalMappedShader(const VkDevice device);

    ~TerrainNormalMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::terrain::shader

#endif