#ifndef __BUMP_MPAAPED_SHADOWS_SHADER_H__
#define __BUMP_MPAAPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::shadow::shader {
class BumpMappedShadowsShader final : public prev::render::shader::Shader {
public:
    BumpMappedShadowsShader(const VkDevice device);

    ~BumpMappedShadowsShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::shadow::shader

#endif // !__BUMP_MPAAPED_SHADOWS_SHADER_H__
