#ifndef __SELECTION_DEBUG_SHADER_H__
#define __SELECTION_DEBUG_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::debug::shader {
class SelectionDebugShader final : public prev::render::shader::Shader {
public:
    SelectionDebugShader(const VkDevice device);

    ~SelectionDebugShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::debug::shader

#endif // !__SELECTION_DEBUG_SHADER_H__
