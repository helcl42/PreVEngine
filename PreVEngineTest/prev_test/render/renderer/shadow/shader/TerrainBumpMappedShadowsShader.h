#ifndef __TERRAIN_BUMP_MAPPED_SHADOWS_SHADER_H__
#define __TERRAIN_BUMP_MAPPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class TerrainBumpMappedShadowsShader final : public prev::render::shader::Shader {
public:
    TerrainBumpMappedShadowsShader(const VkDevice device);

    ~TerrainBumpMappedShadowsShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::shadow::shader

#endif