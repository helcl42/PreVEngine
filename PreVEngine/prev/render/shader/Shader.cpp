#include "Shader.h"

#include "../../common/Logger.h"

#include <algorithm>

namespace prev::render::shader {
Shader::Shader(GfxDevice device,
    const std::map<GfxShaderStageFlags, GfxShader>& shaderModules,
    const std::vector<VertexInputBinding>& vertexBindings,
    const std::vector<VertexInputAttribute>& vertexAttributes,
    GfxBindGroupLayout bindGroupLayout,
    const std::map<std::string, BindingInfo>& bindingInfos)
    : m_device{ device }
    , m_shaderModules{ shaderModules }
    , m_vertexInputBindings{ vertexBindings }
    , m_vertexInputAttributes{ vertexAttributes }
    , m_bindGroupLayout{ bindGroupLayout }
    , m_bindingInfos{ bindingInfos }
{
}

Shader::~Shader()
{
    GFXERRCHECK(gfxDeviceWaitIdle(m_device));

    for (auto& bg : m_bindGroups) {
        if (bg) {
            gfxBindGroupDestroy(bg);
        }
    }
    if (m_bindGroupLayout) {
        gfxBindGroupLayoutDestroy(m_bindGroupLayout);
    }
    for (auto& [stage, shader] : m_shaderModules) {
        gfxShaderDestroy(shader);
    }
}

bool Shader::ShouldAdjustCapacity(uint32_t size) const
{
    const float MIN_CAPACITY_RATIO_TO_SHRINK{ 0.5f };
    if (size > m_poolCapacity) {
        return true;
    }
    if (m_poolCapacity > 0 && float(size) / float(m_poolCapacity) < MIN_CAPACITY_RATIO_TO_SHRINK) {
        return true;
    }
    return false;
}

bool Shader::AdjustBindGroupCapacity(uint32_t size)
{
    if (!ShouldAdjustCapacity(size)) {
        return false;
    }
    for (auto& bg : m_bindGroups) {
        if (bg) {
            gfxBindGroupDestroy(bg);
        }
    }
    m_bindGroups.assign(size, nullptr);
    m_poolCapacity = size;
    m_currentSlot = 0;
    return true;
}

GfxBindGroup Shader::UpdateNextBindGroup()
{
    CheckBindings();

    // Gather entries sorted by binding index (one entry per array element, addressed via arrayElement)
    std::map<std::string, GfxBindGroupEntry> entriesByName;
    for (auto& [name, info] : m_bindingInfos) {
        GfxBindGroupEntry entry{};
        entry.binding = info.binding;
        entry.arrayElement = info.arrayElement;
        entry.type = info.type;
        if (info.type == GFX_BIND_GROUP_ENTRY_TYPE_BUFFER) {
            entry.resource.buffer.buffer = info.buffer;
            entry.resource.buffer.offset = info.bufferOffset;
            entry.resource.buffer.size = info.bufferSize;
        } else if (info.type == GFX_BIND_GROUP_ENTRY_TYPE_TEXTURE_VIEW) {
            entry.resource.textureView = info.textureView;
        } else if (info.type == GFX_BIND_GROUP_ENTRY_TYPE_SAMPLER) {
            entry.resource.sampler = info.sampler;
        }
        entriesByName[name] = entry;
    }

    std::vector<GfxBindGroupEntry> entries;
    entries.reserve(entriesByName.size());
    for (auto& [key, entry] : entriesByName) {
        entries.push_back(entry);
    }

    // Sort entries by binding index to ensure correct descriptor ordering
    std::sort(entries.begin(), entries.end(), [](const GfxBindGroupEntry& a, const GfxBindGroupEntry& b) {
        return std::tie(a.binding, a.arrayElement) < std::tie(b.binding, b.arrayElement);
    });

    if (m_bindGroups[m_currentSlot]) {
        gfxBindGroupDestroy(m_bindGroups[m_currentSlot]);
        m_bindGroups[m_currentSlot] = nullptr;
    }

    GfxBindGroupDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_BIND_GROUP_DESCRIPTOR;
    desc.layout = m_bindGroupLayout;
    desc.entries = entries.data();
    desc.entryCount = static_cast<uint32_t>(entries.size());

    GFXERRCHECK(gfxDeviceCreateBindGroup(m_device, &desc, &m_bindGroups[m_currentSlot]));

    GfxBindGroup result = m_bindGroups[m_currentSlot];
    m_currentSlot = (m_currentSlot + 1) % m_poolCapacity;
    return result;
}

void Shader::Bind(const std::string& name, const prev::render::buffer::Buffer& buffer)
{
    const auto it{ m_bindingInfos.find(name) };
    if (it == m_bindingInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
        return;
    }
    auto& info{ it->second };
    info.buffer = static_cast<GfxBuffer>(buffer);
    info.bufferOffset = buffer.GetOffset();
    info.bufferSize = buffer.GetSize();
}

void Shader::Bind(const std::string& name, const prev::render::buffer::ImageBufferView& imageBufferView)
{
    const auto it{ m_bindingInfos.find(name) };
    if (it == m_bindingInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
        return;
    }
    auto& info{ it->second };
    info.textureView = static_cast<GfxTextureView>(imageBufferView);
}

void Shader::Bind(const std::string& name, const prev::render::sampler::Sampler& sampler)
{
    const auto it{ m_bindingInfos.find(name) };
    if (it == m_bindingInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
        return;
    }
    auto& info{ it->second };
    info.sampler = static_cast<GfxSampler>(sampler);
}

void Shader::CheckBindings() const
{
    for (const auto& [name, info] : m_bindingInfos) {
        if (info.type == GFX_BIND_GROUP_ENTRY_TYPE_BUFFER && !info.buffer) {
            LOGE("Shader item: \"%s\" was not bound. Set a binding before calling UpdateNextBindGroup.", name.c_str());
            PAUSE;
            exit(0);
        }
    }
}

const std::map<GfxShaderStageFlags, GfxShader>& Shader::GetShaderModules() const
{
    return m_shaderModules;
}

GfxBindGroupLayout Shader::GetBindGroupLayout() const
{
    return m_bindGroupLayout;
}

const std::vector<VertexInputBinding>& Shader::GetVertexInputBindings() const
{
    return m_vertexInputBindings;
}

const std::vector<VertexInputAttribute>& Shader::GetVertexInputAttributes() const
{
    return m_vertexInputAttributes;
}
} // namespace prev::render::shader
