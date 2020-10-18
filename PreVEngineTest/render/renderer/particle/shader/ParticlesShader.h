#ifndef __PARTICLES_SHADER_H__
#define __PARTICLES_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::particle::shader {
class ParticlesShader final : public prev::render::shader::Shader {
public:
    ParticlesShader(const VkDevice device);

    ~ParticlesShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::particle::shader

#endif