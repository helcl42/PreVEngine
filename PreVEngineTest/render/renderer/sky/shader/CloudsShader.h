#ifndef __CLOUDS_SHADER_H__
#define __CLOUDS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class CloudsShader final : public prev::render::shader::Shader {
public:
    CloudsShader(const VkDevice device);

    ~CloudsShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__CLOUDS_SHADER_H__
