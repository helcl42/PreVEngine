#ifndef __NORRMAL_MAPPED_SHADER_H__
#define __NORRMAL_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::normal::shader {
class NormalMappedShader final : public prev::render::shader::Shader {
public:
    NormalMappedShader(const VkDevice device);

    ~NormalMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::normal::shader

#endif