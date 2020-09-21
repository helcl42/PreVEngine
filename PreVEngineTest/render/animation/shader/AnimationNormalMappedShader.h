#ifndef __ANIMATION_NORMAL_MAPPED_SHADER_H__
#define __ANIMATION_NORMAL_MAPPED_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::animation::shader {
class AnimationNormalMappedShader final : public prev::render::shader::Shader {
public:
    AnimationNormalMappedShader(const VkDevice device);

    ~AnimationNormalMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
}

#endif