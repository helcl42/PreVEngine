#ifndef __FLARE_SHADER_H__
#define __FLARE_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::sky::shader {
class FlareShader final : public prev::render::shader::Shader {
public:
    FlareShader(const VkDevice device);

    ~FlareShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::sky::shader

#endif // !__FLARE_SHADER_H__
