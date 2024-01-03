#ifndef __ANIMATION_TEXTURELESS_SHADER_H__
#define __ANIMATION_TEXTURELESS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::animation::shader {
class AnimationTexturelessShader final : public prev::render::shader::Shader {
public:
    AnimationTexturelessShader(const VkDevice device);

    ~AnimationTexturelessShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::animation::shader

#endif // !__ANIMATION_TEXTURELESS_SHADER_H__
