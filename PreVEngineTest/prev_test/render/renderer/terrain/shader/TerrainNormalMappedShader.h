#ifndef __TERRAIN_NORMAL_SHADER_H__
#define __TERRAIN_NORMAL_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::terrain::shader {
class TerrainNormalMappedShader final : public prev::render::shader::Shader {
public:
    TerrainNormalMappedShader(const VkDevice device);

    ~TerrainNormalMappedShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::terrain::shader

#endif