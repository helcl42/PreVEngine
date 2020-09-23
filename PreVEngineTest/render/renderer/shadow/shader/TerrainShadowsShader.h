#ifndef __TERRAIN_SHADOWS_SHADER_H__
#define __TERRAIN_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class TerrainShadowsShader final : public prev::render::shader::Shader {
public:
    TerrainShadowsShader(const VkDevice device);

    ~TerrainShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::shadow::shader

#endif // !__TERRAIN_SHADOWS_SHADER_H__
