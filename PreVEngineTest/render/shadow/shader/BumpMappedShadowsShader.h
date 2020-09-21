#ifndef __BUMP_MPAAPED_SHADOWS_SHADER_H__
#define __BUMP_MPAAPED_SHADOWS_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::shadow::shader {
class BumpMappedShadowsShader final : public prev::render::shader::Shader {
public:
    BumpMappedShadowsShader(const VkDevice device);

    ~BumpMappedShadowsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::shadow::shader

#endif // !__BUMP_MPAAPED_SHADOWS_SHADER_H__
