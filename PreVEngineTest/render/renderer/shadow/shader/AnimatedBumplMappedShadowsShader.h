#ifndef __ANIMATED_BUMP_MAPPED_SHADOWS_SHADER_H__
#define __ANIMATED_BUMP_MAPPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class AnimatedBumplMappedShadowsShader final : public prev::render::shader::Shader {
public:
    AnimatedBumplMappedShadowsShader(const VkDevice device);

    ~AnimatedBumplMappedShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};

} // namespace prev_test::render::renderer::shadow::shader

#endif