#ifndef __SHADOW_MAP_DEBUG_SHADER_H__
#define __SHADOW_MAP_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class ShadowMapDebugShader final : public prev::render::shader::Shader {
public:
    ShadowMapDebugShader(const VkDevice device);

    ~ShadowMapDebugShader() = default;

public:
    static std::map<VkShaderStageFlagBits, std::string> GetPaths();

private:
    std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const override;

    std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const override;

    std::vector<DescriptorSet> CreateDescriptorSets() const override;

    std::vector<PushConstantBlock> CreatePushConstantBlocks() const override;
};
} // namespace prev_test::render::renderer::debug::shader

#endif