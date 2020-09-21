#ifndef __ANIMATION_PARALLAX_MAPPED_SHADER_H__
#define __ANIMATION_PARALLAX_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::animation::shader {
class AnimationParallaxMappedShader final : public prev::render::shader::Shader {
public:
    AnimationParallaxMappedShader(const VkDevice device);

    ~AnimationParallaxMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::animation::shader

#endif