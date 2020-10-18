#ifndef __CLOUDS_PERLIN_WORLEY_NOISE_SHADER_H__
#define __CLOUDS_PERLIN_WORLEY_NOISE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class CloudsPerlinWorleyNoiseShader final : public prev::render::shader::Shader {
public:
    CloudsPerlinWorleyNoiseShader(const VkDevice device);

    ~CloudsPerlinWorleyNoiseShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__PERLIN_WORLEY_NOISE_SHADER_H__
