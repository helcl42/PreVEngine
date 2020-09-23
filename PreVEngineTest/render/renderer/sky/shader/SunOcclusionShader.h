#ifndef __SUN_OCCLUSION_SHADER_H__
#define __SUN_OCCLUSION_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class SunOcclusionShader final : public prev::render::shader::Shader {
public:
    SunOcclusionShader(const VkDevice device);

    ~SunOcclusionShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif