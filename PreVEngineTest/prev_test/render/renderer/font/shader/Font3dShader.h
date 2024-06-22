#ifndef __FONT_3D_SHADER_H__
#define __FONT_3D_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::font::shader {
class Font3dShader final : public prev::render::shader::Shader {
public:
    Font3dShader(const VkDevice device);

    ~Font3dShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::font::shader

#endif // !__FONT_3D_SHADER_H__
