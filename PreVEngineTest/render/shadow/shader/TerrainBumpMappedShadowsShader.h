#ifndef __TERRAIN_BUMP_MAPPED_SHADOWS_SHADER_H__
#define __TERRAIN_BUMP_MAPPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::shadow::shader {
class TerrainBumpMappedShadowsShader final : public prev::render::shader::Shader {
public:
    TerrainBumpMappedShadowsShader(const VkDevice device);

    ~TerrainBumpMappedShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::shadow::shader

#endif