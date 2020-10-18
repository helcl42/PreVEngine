#ifndef __ANIMATION_PARALLAX_MAPPED_SHADER_H__
#define __ANIMATION_PARALLAX_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::animation::shader {
class AnimationParallaxMappedShader final : public prev::render::shader::Shader {
public:
    AnimationParallaxMappedShader(const VkDevice device);

    ~AnimationParallaxMappedShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::animation::shader

#endif