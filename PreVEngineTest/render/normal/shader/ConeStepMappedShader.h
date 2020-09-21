#ifndef __CONE_STEM_MAPPED_SHADER_H__
#define __CONE_STEM_MAPPED_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::normal::shader {
class ConeStepMappedShader final : public prev::render::shader::Shader {
public:
    ConeStepMappedShader(const VkDevice device);

    ~ConeStepMappedShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::normal::shader

#endif // !__CONE_STEM_MAPPED_SHADER_H__
