#ifndef __SKY_BOX_SHADER_H__
#define __SKY_BOX_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::sky::shader {
class SkyBoxShader final : public prev::render::shader::Shader {
public:
    SkyBoxShader(const VkDevice device);

    ~SkyBoxShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::sky::shader

#endif // !__SKY_BOX_SHADER_H__
