#ifndef __SANDBOX_DEFAULT_RENDERER_H__
#define __SANDBOX_DEFAULT_RENDERER_H__

#include <prev/core/device/Device.h>
#include <prev/render/buffer/FrameScopedBufferPool.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include "../common/Common.h"

#include <glm/glm.hpp>

#include <array>
#include <memory>

namespace sandbox::render {
// Per-frame camera matrices, one entry per view (a single view in mono, one per eye under XR).
// View and projection are kept separate (not pre-combined) so the shader can work in world/view
// space; the renderer pairs them with each node's model matrix.
struct ViewData {
    std::array<glm::mat4, MAX_VIEW_COUNT_VALUE> viewMatrices{};
    std::array<glm::mat4, MAX_VIEW_COUNT_VALUE> projectionMatrices{};
    uint32_t viewCount{ 1 };
};

class DefaultRenderer final {
public:
    DefaultRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass);

    ~DefaultRenderer() = default;

public:
    void Init();

    void BeginFrame(uint32_t frameInFlightIndex);

    void EndFrame();

    void Render(GfxRenderPassEncoder encoder, const GfxScissorRect& rect, const ViewData& views, const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

    void ShutDown();

private:
    // std140-compatible: all members are 16-byte aligned (mat4 = 64, vec4 = 16), no padding needed.
    // view/projection are arrays (1 in mono, 2 for stereo XR); the vertex shader picks its view by
    // SV_ViewID and composes projection * view * model itself.
    struct alignas(16) SandboxUniforms {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
        glm::mat4 viewMatrices[MAX_VIEW_COUNT_VALUE];
        glm::mat4 projectionMatrices[MAX_VIEW_COUNT_VALUE];
        glm::vec4 lightDirection;
        glm::vec4 color;
    };

    // Uniform pool growth granularity (slices per allocated chunk). Not a hard cap: the pool is
    // frame-scoped and grows on demand, so any value is correct — this only tunes allocation size.
    static constexpr uint32_t m_uniformPoolChunk{ 16 };

    prev::core::device::Device& m_device;
    prev::render::pass::RenderPass& m_renderPass;

    std::unique_ptr<prev::render::shader::Shader> m_shader;
    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;
    std::unique_ptr<prev::render::buffer::FrameScopedBufferPool> m_uniformsPool;
};
} // namespace sandbox::render

#endif // !__SANDBOX_DEFAULT_RENDERER_H__
