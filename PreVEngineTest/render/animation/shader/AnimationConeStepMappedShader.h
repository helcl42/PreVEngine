#ifndef __ANIMATION_CONE_STEP_MAPPED_SHADER_H__
#define __ANIMATION_CONE_STEP_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::animation::shader {
class AnimationConeStepMappedShader final : public prev::render::shader::Shader {
public:
    AnimationConeStepMappedShader(const VkDevice device);

    ~AnimationConeStepMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::animation::shader

#endif