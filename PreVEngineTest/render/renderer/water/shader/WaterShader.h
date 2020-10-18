#ifndef __WATER_SHADER_H__
#define __WATER_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::water::shader {
class WaterShader final : public prev::render::shader::Shader {
public:
    WaterShader(const VkDevice device);

    ~WaterShader() = default;

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};

} // namespace prev_test::render::renderer::water::shader

#endif // !__WATER_SHADER_H__
