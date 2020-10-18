#ifndef __TERRAIN_PARALLAX_MAPPED_SHADER_H__
#define __TERRAIN_PARALLAX_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::terrain::shader {
class TerrainParallaxMappedShader final : public prev::render::shader::Shader {
public:
    TerrainParallaxMappedShader(const VkDevice device);

    ~TerrainParallaxMappedShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::terrain::shader

#endif