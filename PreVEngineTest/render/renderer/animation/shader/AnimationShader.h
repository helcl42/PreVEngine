#ifndef __ANIMATION_SHADER_H__
#define __ANIMATION_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::animation::shader {
class AnimationShader final : public prev::render::shader::Shader {
public:
    AnimationShader(const VkDevice device);

    ~AnimationShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::animation::shader

#endif // !__ANIMATION_SHADER_H__
