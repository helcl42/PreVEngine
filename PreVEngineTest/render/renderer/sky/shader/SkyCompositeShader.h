#ifndef __SKY_COMPOSITE_SHADER_H__
#define __SKY_COMPOSITE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class SkyCompositeShader final : public prev::render::shader::Shader {
public:
    SkyCompositeShader(const VkDevice device);

    ~SkyCompositeShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__SKY_COMPOSITE_SHADER_H__
