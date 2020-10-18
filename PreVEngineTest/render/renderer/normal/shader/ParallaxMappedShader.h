#ifndef __PARALLAX_MAPPED_SHADER_H__
#define __PARALLAX_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::normal::shader {
class ParallaxMappedShader final : public prev::render::shader::Shader {
public:
    ParallaxMappedShader(const VkDevice device);

    ~ParallaxMappedShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::normal::shader

#endif // !__PARALLAX_MAPPED_SHADER_H__
