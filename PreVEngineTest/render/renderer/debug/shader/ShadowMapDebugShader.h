#ifndef __SHADOW_MAP_DEBUG_SHADER_H__
#define __SHADOW_MAP_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class ShadowMapDebugShader final : public prev::render::shader::Shader {
public:
    ShadowMapDebugShader(const VkDevice device);

    ~ShadowMapDebugShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::debug::shader

#endif