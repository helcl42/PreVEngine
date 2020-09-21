#ifndef __DEFAULT_SHADOWS_SHADER_H__
#define __DEFAULT_SHADOWS_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::shadow::shader {
class DefaultShadowsShader final : public prev::render::shader::Shader {
public:
    DefaultShadowsShader(const VkDevice device);

    ~DefaultShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::shadow::shader

#endif // !__DEFAULT_SHADOWS_SHADER_H__
