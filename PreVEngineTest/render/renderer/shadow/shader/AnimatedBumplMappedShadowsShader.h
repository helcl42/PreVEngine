#ifndef __ANIMATED_BUMP_MAPPED_SHADOWS_SHADER_H__
#define __ANIMATED_BUMP_MAPPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class AnimatedBumplMappedShadowsShader final : public prev::render::shader::Shader {
public:
    AnimatedBumplMappedShadowsShader(const VkDevice device);

    ~AnimatedBumplMappedShadowsShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};

} // namespace prev_test::render::renderer::shadow::shader

#endif