#ifndef __CONE_STEM_MAPPED_SHADER_H__
#define __CONE_STEM_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::normal::shader {
class ConeStepMappedShader final : public prev::render::shader::Shader {
public:
    ConeStepMappedShader(const VkDevice device);

    ~ConeStepMappedShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::normal::shader

#endif // !__CONE_STEM_MAPPED_SHADER_H__
