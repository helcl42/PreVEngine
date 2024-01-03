#ifndef __TERRAIN_SHADOWS_SHADER_H__
#define __TERRAIN_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class TerrainShadowsShader final : public prev::render::shader::Shader {
public:
    TerrainShadowsShader(const VkDevice device);

    ~TerrainShadowsShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::shadow::shader

#endif // !__TERRAIN_SHADOWS_SHADER_H__
