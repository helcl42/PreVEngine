#ifndef __SKY_SHADER_H__
#define __SKY_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class SkyShader final : public prev::render::shader::Shader {
public:
    SkyShader(const VkDevice device);

    ~SkyShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__SKY_SHADER_H__
