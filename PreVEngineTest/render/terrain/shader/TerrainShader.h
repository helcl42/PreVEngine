#ifndef __TERRAIN_SHADER_H__
#define __TERRAIN_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::terrain::shader {
class TerrainShader final : public prev::render::shader::Shader {
public:
    TerrainShader(const VkDevice device);

    ~TerrainShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::terrain::shader

#endif // !__TERRAIN_SHADER_H__
