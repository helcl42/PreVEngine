#ifndef __TEXTURELESS_SHADER_H__
#define __TEXTURELESS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::normal::shader {
class TexturelessShader final : public prev::render::shader::Shader {
public:
    TexturelessShader(const VkDevice device);

    ~TexturelessShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::normal::shader

#endif // !__TEXTURELESS_SHADER_H__
