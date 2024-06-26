#ifndef __FONT_SHADER_H__
#define __FONT_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::font::shader {
class FontShader final : public prev::render::shader::Shader {
public:
    FontShader(const VkDevice device);

    ~FontShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::font::shader

#endif // !__FONT_SHADER_H__
