#ifndef __SHADER_H__
#define __SHADER_H__

#include "../buffer/Buffer.h"
#include "../buffer/ImageBufferView.h"
#include "../sampler/Sampler.h"

#include "../../core/Core.h"

#include <map>
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
    Shader(GfxDevice device,
        const std::map<GfxShaderStageFlags, GfxShader>& shaderModules,
        const std::vector<VertexInputBinding>& vertexBindings,
        const std::vector<VertexInputAttribute>& vertexAttributes,
        GfxBindGroupLayout bindGroupLayout,
        const std::map<std::string, BindingInfo>& bindingInfos);

public:
    ~Shader();

public:
    bool AdjustBindGroupCapacity(uint32_t size);

    void Bind(const std::string& name, const prev::render::buffer::Buffer& buffer);

    void Bind(const std::string& name, const prev::render::buffer::ImageBufferView& imageBufferView);

    void Bind(const std::string& name, const prev::render::sampler::Sampler& sampler);

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

    bool ShouldAdjustCapacity(uint32_t size) const;

private:
    GfxDevice m_device;

    std::map<GfxShaderStageFlags, GfxShader> m_shaderModules;

    std::vector<VertexInputBinding> m_vertexInputBindings;

    std::vector<VertexInputAttribute> m_vertexInputAttributes;

    GfxBindGroupLayout m_bindGroupLayout{};

    std::map<std::string, BindingInfo> m_bindingInfos;

    std::vector<GfxBindGroup> m_bindGroups;

    uint32_t m_poolCapacity{};

    uint32_t m_currentSlot{};
};

} // namespace prev::render::shader

#endif
