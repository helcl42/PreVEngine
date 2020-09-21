#ifndef __ANIMATION_SHADER_H__
#define __ANIMATION_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::animation::shader {
class AnimationShader final : public prev::render::shader::Shader {
public:
    AnimationShader(const VkDevice device);

    ~AnimationShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::animation::shader

#endif // !__ANIMATION_SHADER_H__
