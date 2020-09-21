#ifndef __CLOUDS_SHADER_H__
#define __CLOUDS_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::sky::shader {
class CloudsShader final : public prev::render::shader::Shader {
public:
    CloudsShader(const VkDevice device);

    ~CloudsShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::sky::shader

#endif // !__CLOUDS_SHADER_H__
