#ifndef __SELECTION_DEBUG_SHADER_H__
#define __SELECTION_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class SelectionDebugShader final : public prev::render::shader::Shader {
public:
    SelectionDebugShader(const VkDevice device);

    ~SelectionDebugShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::debug::shader

#endif // !__SELECTION_DEBUG_SHADER_H__
