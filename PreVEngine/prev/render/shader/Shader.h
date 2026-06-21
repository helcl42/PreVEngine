#ifndef __SHADER_H__
#define __SHADER_H__

#include "IBindGroupPool.h"

#include "../buffer/Buffer.h"
#include "../buffer/ImageBufferView.h"
#include "../sampler/Sampler.h"

#include "../../core/Core.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder;

// Describes a single vertex buffer slot
struct VertexInputBinding {
    uint32_t binding{};
    uint32_t stride{};
    GfxVertexStepMode stepMode{ GFX_VERTEX_STEP_MODE_VERTEX };
};

// Describes a single vertex attribute within a buffer slot
struct VertexInputAttribute {
    uint32_t binding{};
    uint32_t shaderLocation{};
    GfxFormat format{ GFX_FORMAT_UNDEFINED };
    uint32_t offset{};
};

class Shader final {
public:
    struct BindingInfo {
        uint32_t binding{};
        uint32_t arrayElement{};
        GfxBindGroupEntryType type{};
        // Buffer
        GfxBuffer buffer{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        // Combined image + sampler
        GfxTextureView textureView{};
        GfxSampler sampler{};
    };

private:
    // The bind-group pool (ring vs frame-scoped) is chosen by the builder and injected here, so the
    // shader depends only on the IBindGroupPool interface, not the concrete strategies.
    Shader(GfxDevice device,
        const std::map<GfxShaderStageFlags, GfxShader>& shaderModules,
        const std::vector<VertexInputBinding>& vertexBindings,
        const std::vector<VertexInputAttribute>& vertexAttributes,
        GfxBindGroupLayout bindGroupLayout,
        const std::map<std::string, BindingInfo>& bindingInfos,
        std::unique_ptr<IBindGroupPool> bindGroupPool);

public:
    ~Shader();

public:
    void Bind(const std::string& name, const prev::render::buffer::Buffer& buffer);

    void Bind(const std::string& name, const prev::render::buffer::ImageBufferView& imageBufferView);

    void Bind(const std::string& name, const prev::render::sampler::Sampler& sampler);

    // Frame-scoped pools only (built with capacity 0): call once per frame with the swapchain's
    // frame-in-flight index. UpdateNextBindGroup() then draws from that frame's grow-on-demand
    // region; reuse of a frame index is fenced by the swapchain, so recreating its bind groups is
    // safe. No-ops for a fixed-ring shader.
    void BeginFrame(uint32_t frameInFlightIndex);

    // Optional pair to BeginFrame: trims this frame's region back to what it actually used so a
    // one-off object spike doesn't pin memory forever. No-ops for a fixed-ring shader.
    void EndFrame();

    GfxBindGroup UpdateNextBindGroup();

public:
    const std::map<GfxShaderStageFlags, GfxShader>& GetShaderModules() const;

    GfxBindGroupLayout GetBindGroupLayout() const;

    const std::vector<VertexInputBinding>& GetVertexInputBindings() const;

    const std::vector<VertexInputAttribute>& GetVertexInputAttributes() const;

public:
    friend class ShaderBuilder;

private:
    void CheckBindings() const;

private:
    GfxDevice m_device;

    std::map<GfxShaderStageFlags, GfxShader> m_shaderModules;

    std::vector<VertexInputBinding> m_vertexInputBindings;

    std::vector<VertexInputAttribute> m_vertexInputAttributes;

    GfxBindGroupLayout m_bindGroupLayout{};

    std::map<std::string, BindingInfo> m_bindingInfos;

    // Owns the bind-group slots: a fixed ring or a frame-scoped pool, chosen at construction.
    std::unique_ptr<IBindGroupPool> m_bindGroupPool;
};

} // namespace prev::render::shader

#endif
