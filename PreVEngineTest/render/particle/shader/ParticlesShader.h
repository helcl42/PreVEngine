#ifndef __PARTICLES_SHADER_H__
#define __PARTICLES_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::particle::shader {
class ParticlesShader final : public prev::render::shader::Shader {
public:
    ParticlesShader(const VkDevice device);

    ~ParticlesShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::particle::shader

#endif