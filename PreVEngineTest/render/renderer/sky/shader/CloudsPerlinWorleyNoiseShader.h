#ifndef __CLOUDS_PERLIN_WORLEY_NOISE_SHADER_H__
#define __CLOUDS_PERLIN_WORLEY_NOISE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class CloudsPerlinWorleyNoiseShader final : public prev::render::shader::Shader {
public:
    CloudsPerlinWorleyNoiseShader(const VkDevice device);

    ~CloudsPerlinWorleyNoiseShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__PERLIN_WORLEY_NOISE_SHADER_H__
