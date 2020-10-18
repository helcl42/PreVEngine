#ifndef __SCREEN_SPACE_SHADER_H__
#define __SCREEN_SPACE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::common::shader {
class ScreenSpaceShader final : public prev::render::shader::Shader {
public:
    ScreenSpaceShader(const VkDevice device);

    ~ScreenSpaceShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::common::shader

#endif // !__SCREEN_SPACE_SHADER_H__
