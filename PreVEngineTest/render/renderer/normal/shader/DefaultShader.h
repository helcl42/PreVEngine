#ifndef __DEFAULT_SHADER_H__
#define __DEFAULT_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::normal::shader {
class DefaultShader final : public prev::render::shader::Shader {
public:
    DefaultShader(const VkDevice device);

    ~DefaultShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::normal::shader

#endif // !__DEFAULT_SHADER_H__
