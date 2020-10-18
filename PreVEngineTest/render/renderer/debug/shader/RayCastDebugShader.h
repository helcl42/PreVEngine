#ifndef __RAY_CAST_DEBUG_SHADER_H__
#define __RAY_CAST_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class RayCastDebugShader final : public prev::render::shader::Shader {
public:
    RayCastDebugShader(const VkDevice device);

    ~RayCastDebugShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::debug::shader

#endif