#ifndef __PARALLAX_MAPPED_SHADER_H__
#define __PARALLAX_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::normal::shader {
class ParallaxMappedShader final : public prev::render::shader::Shader {
public:
    ParallaxMappedShader(const VkDevice device);

    ~ParallaxMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::normal::shader

#endif // !__PARALLAX_MAPPED_SHADER_H__
