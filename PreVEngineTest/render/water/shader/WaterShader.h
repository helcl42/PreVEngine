#ifndef __WATER_SHADER_H__
#define __WATER_SHADER_H__

#include <render/shader/Shader.h>

namespace prev_test::render::water::shader {
class WaterShader final : public prev::render::shader::Shader {
public:
    WaterShader(const VkDevice device);

    ~WaterShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};

} // namespace prev_test::render::water::shader

#endif // !__WATER_SHADER_H__
