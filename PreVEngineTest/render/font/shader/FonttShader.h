#ifndef __FONT_SHADER_H__
#define __FONT_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::font::shader {
class FonttShader final : public prev::render::shader::Shader {
public:
    FonttShader(const VkDevice device);

    ~FonttShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::font::shader

#endif // !__FONT_SHADER_H__
