#include "ShaderBuilder.h"

#include "../../common/Logger.h"
#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <stdexcept>

namespace prev::render::shader {
ShaderBuilder::ShaderBuilder(GfxDevice device)
    : m_device{ device }
{
}

ShaderBuilder& ShaderBuilder::AddShaderStagePath(GfxShaderStageFlags stage, const std::string& path)
{
    m_stagePaths.insert({ stage, path });
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStagePaths(const std::map<GfxShaderStageFlags, std::string>& stagePaths)
{
    for (const auto& [stage, path] : stagePaths) {
        AddShaderStagePath(stage, path);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStageByteCode(GfxShaderStageFlags stage, const std::vector<char>& byteCode)
{
    m_stageByteCodes.insert({ stage, byteCode });
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStageByteCodes(const std::map<GfxShaderStageFlags, std::vector<char>>& byteCodes)
{
    for (const auto& [stage, byteCode] : byteCodes) {
        AddShaderStageByteCode(stage, byteCode);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputBinding(const VertexInputBinding& desc)
{
    m_vertexInputBindings.push_back(desc);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputBindings(const std::vector<VertexInputBinding>& descs)
{
    for (const auto& desc : descs) {
        AddVertexInputBinding(desc);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputAttribute(const VertexInputAttribute& desc)
{
    m_vertexInputAttributes.push_back(desc);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputAttributes(const std::vector<VertexInputAttribute>& descs)
{
    for (const auto& desc : descs) {
        AddVertexInputAttribute(desc);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddDescriptorSet(const DescriptorSet& descriptorSet)
{
    m_descriptorSets.push_back(descriptorSet);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddDescriptorSets(const std::vector<DescriptorSet>& descriptorSets)
{
    for (const auto& ds : descriptorSets) {
        AddDescriptorSet(ds);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::SetBindGroupCapacity(uint32_t size)
{
    m_bindGroupCapacity = size;
    return *this;
}

ShaderBuilder& ShaderBuilder::SetEntryPointName(const std::string& name)
{
    m_entryPointName = name;
    return *this;
}

std::unique_ptr<Shader> ShaderBuilder::Build() const
{
    // Determine preferred shader format
    // Prefer WGSL when supported (avoids Dawn Tint SPIR-V reader issues on WebGPU)
    bool useWgsl{ false };
    bool wgslSupported{ false };
    GfxResult result = gfxDeviceSupportsShaderFormat(m_device, GFX_SHADER_SOURCE_WGSL, &wgslSupported);
    LOGI("ShaderBuilder: gfxDeviceSupportsShaderFormat result=%d, wgslSupported=%d", static_cast<int>(result), static_cast<int>(wgslSupported));
    if (result == GFX_RESULT_SUCCESS && wgslSupported) {
        useWgsl = true;
        LOGI("ShaderBuilder: Using WGSL shader format");
    }

    // Build shader modules
    auto byteCodes{ m_stageByteCodes };
    for (const auto& [stage, path] : m_stagePaths) {
        std::string actualPath = path;
        if (useWgsl) {
            // Replace .spv extension with .wgsl
            const auto pos = actualPath.rfind(".spv");
            if (pos != std::string::npos) {
                actualPath.replace(pos, 4, ".wgsl");
            }
        }
        byteCodes.insert({ stage, prev::util::file::ReadBinaryFile(actualPath) });
    }

    std::map<GfxShaderStageFlags, GfxShader> shaderModules;
    for (const auto& [stage, byteCode] : byteCodes) {
        shaderModules[stage] = CreateShaderModule(byteCode, useWgsl ? GFX_SHADER_SOURCE_WGSL : GFX_SHADER_SOURCE_SPIRV);
    }

    // Build bind group layout
    GfxBindGroupLayout bindGroupLayout{ CreateBindGroupLayout() };

    // Build binding infos
    std::map<std::string, Shader::BindingInfo> bindingInfos;
    for (const auto& ds : m_descriptorSets) {
        const auto entryType = [](GfxBindingType bt) -> GfxBindGroupEntryType {
            switch (bt) {
            case GFX_BINDING_TYPE_BUFFER:
                return GFX_BIND_GROUP_ENTRY_TYPE_BUFFER;
            case GFX_BINDING_TYPE_SAMPLER:
                return GFX_BIND_GROUP_ENTRY_TYPE_SAMPLER;
            default:
                return GFX_BIND_GROUP_ENTRY_TYPE_TEXTURE_VIEW;
            }
        }(ds.bindingType);

        if (ds.count > 1) {
            for (uint32_t i = 0; i < ds.count; ++i) {
                Shader::BindingInfo info{};
                info.binding = ds.binding;
                info.type = entryType;
                bindingInfos[ds.name + "[" + std::to_string(i) + "]"] = info;
            }
        } else {
            Shader::BindingInfo info{};
            info.binding = ds.binding;
            info.type = entryType;
            bindingInfos[ds.name] = info;
        }
    }

    auto shader{ std::unique_ptr<Shader>(new Shader(
        m_device,
        std::move(shaderModules),
        m_vertexInputBindings,
        m_vertexInputAttributes,
        bindGroupLayout,
        std::move(bindingInfos))) };

    if (m_bindGroupCapacity > 0) {
        shader->AdjustBindGroupCapacity(m_bindGroupCapacity);
    }
    return shader;
}

GfxShader ShaderBuilder::CreateShaderModule(const std::vector<char>& code, GfxShaderSourceType sourceType) const
{
    GfxShaderDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_SHADER_DESCRIPTOR;
    desc.sourceType = sourceType;
    desc.entryPoint = m_entryPointName.empty() ? DEFAULT_ENTRY_POINT_NAME.c_str() : m_entryPointName.c_str();

    std::vector<uint32_t> codeAligned;
    if (sourceType == GFX_SHADER_SOURCE_SPIRV) {
        codeAligned.resize(prev::util::math::RoundUp<size_t>(code.size() / 4, 4));
        memcpy(codeAligned.data(), code.data(), code.size());
        desc.code = codeAligned.data();
        desc.codeSize = code.size();
    } else {
        // WGSL is text — pass as-is
        desc.code = code.data();
        desc.codeSize = code.size();
    }

    GfxShader shader{};
    GFXERRCHECK(gfxDeviceCreateShader(m_device, &desc, &shader));
    return shader;
}

GfxBindGroupLayout ShaderBuilder::CreateBindGroupLayout() const
{
    std::vector<GfxBindGroupLayoutEntry> entries;
    for (const auto& ds : m_descriptorSets) {
        GfxBindGroupLayoutEntry entry{};
        entry.binding = ds.binding;
        entry.visibility = ds.stageFlags;
        entry.type = ds.bindingType;
        entry.count = ds.count;
        if (entry.type == GFX_BINDING_TYPE_TEXTURE) {
            entry.texture.viewDimension = ds.textureViewDimension;
            entry.texture.sampleType = ds.textureSampleType;
        } else if (entry.type == GFX_BINDING_TYPE_SAMPLER) {
            entry.sampler.type = ds.samplerNonFiltering ? GFX_SAMPLER_BINDING_TYPE_NON_FILTERING : GFX_SAMPLER_BINDING_TYPE_FILTERING;
        } else if (entry.type == GFX_BINDING_TYPE_STORAGE_TEXTURE) {
            entry.storageTexture.format = ds.storageTextureFormat;
            entry.storageTexture.viewDimension = ds.storageTextureViewDimension;
            entry.storageTexture.access = ds.storageTextureAccess;
        }
        entries.push_back(entry);
    }

    GfxBindGroupLayoutDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_BIND_GROUP_LAYOUT_DESCRIPTOR;
    desc.entries = entries.data();
    desc.entryCount = static_cast<uint32_t>(entries.size());

    GfxBindGroupLayout layout{};
    GFXERRCHECK(gfxDeviceCreateBindGroupLayout(m_device, &desc, &layout));
    return layout;
}

} // namespace prev::render::shader
