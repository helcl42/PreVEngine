#ifndef __DEFAULT_SHADER_H__
#define __DEFAULT_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::normal::shader {
class DefaultShader final : public prev::render::shader::Shader {
public:
    DefaultShader(const VkDevice device);

    ~DefaultShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::normal::shader

#endif // !__DEFAULT_SHADER_H__
