#ifndef __TEXTURE_DEBUG_SHADER_H__
#define __TEXTURE_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class TextureDebugShader final : public prev::render::shader::Shader {
public:
    TextureDebugShader(const VkDevice device);

    ~TextureDebugShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};

} // namespace prev_test::render::renderer::debug::shader

#endif