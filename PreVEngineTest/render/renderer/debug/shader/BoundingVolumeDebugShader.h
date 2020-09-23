#ifndef __BOUNDING_VOLUME_DEBUG_SHADER_H__
#define __BOUNDING_VOLUME_DEBUG_SHADER_H__

#include <prev/render/shader/Shader.h>

namespace prev_test::render::renderer::debug::shader {
class BoundingVolumeDebugShader final : public prev::render::shader::Shader {
public:
    BoundingVolumeDebugShader(const VkDevice device);

    ~BoundingVolumeDebugShader() = default;

private:
    void InitVertexInputs() override;

    void InitDescriptorSets() override;

    void InitPushConstantsBlocks() override;
};
} // namespace prev_test::render::renderer::debug::shader

#endif // !__BOUNDING_VOLUME_DEBUG_SHADER_H__
