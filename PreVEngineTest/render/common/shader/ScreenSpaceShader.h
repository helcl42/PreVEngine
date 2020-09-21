#ifndef __SCREEN_SPACE_SHADER_H__
#define __SCREEN_SPACE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::common::shader {
class ScreenSpaceShader final : public prev::render::shader::Shader {
public:
    ScreenSpaceShader(const VkDevice device);

    ~ScreenSpaceShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::common::shader

#endif // !__SCREEN_SPACE_SHADER_H__
