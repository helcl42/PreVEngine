#ifndef __SKY_SHADER_H__
#define __SKY_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::sky::shader {
class SkyShader final : public prev::render::shader::Shader {
public:
    SkyShader(const VkDevice device);

    ~SkyShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::sky::shader

#endif // !__SKY_SHADER_H__
