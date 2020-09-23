#ifndef __ANIMATED_SHADOWS_SHADER_H__
#define __ANIMATED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class AnimatedShadowsShader final : public prev::render::shader::Shader {
public:
    AnimatedShadowsShader(const VkDevice device);

    ~AnimatedShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};

} // namespace prev_test::render::renderer::shadow::shader

#endif // !__ANIMATED_SHADOWS_SHADER_H__
