#ifndef __SHADER_BUILDER_H__
#define __SHADER_BUILDER_H__

#include "Shader.h"

#include <map>
#include <memory>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder final {
public:
    struct BindGroupEntry {
        std::string name{};
        uint32_t binding{};
        GfxBindingType bindingType{};
        GfxShaderStageFlags stageFlags{};
        bool optional{};
        uint32_t count{ 1 };
        GfxFormat storageTextureFormat{ GFX_FORMAT_UNDEFINED };
        GfxTextureViewType storageTextureViewDimension{ GFX_TEXTURE_VIEW_TYPE_2D };
        GfxStorageTextureAccess storageTextureAccess{ GFX_STORAGE_TEXTURE_ACCESS_WRITE_ONLY };
        GfxTextureViewType textureViewDimension{ GFX_TEXTURE_VIEW_TYPE_2D };
        GfxTextureSampleType textureSampleType{ GFX_TEXTURE_SAMPLE_TYPE_FLOAT };
        bool samplerNonFiltering{};

        static BindGroupEntry Buffer(const std::string& name, uint32_t binding, GfxShaderStageFlags stageFlags)
        {
            BindGroupEntry bge{};
            bge.name = name;
            bge.binding = binding;
            bge.bindingType = GFX_BINDING_TYPE_BUFFER;
            bge.stageFlags = stageFlags;
            return bge;
        }

        static BindGroupEntry Texture(const std::string& name, uint32_t binding, GfxShaderStageFlags stageFlags, GfxTextureViewType viewDimension, uint32_t count = 1, GfxTextureSampleType sampleType = GFX_TEXTURE_SAMPLE_TYPE_FLOAT)
        {
            BindGroupEntry bge{};
            bge.name = name;
            bge.binding = binding;
            bge.bindingType = GFX_BINDING_TYPE_TEXTURE;
            bge.stageFlags = stageFlags;
            bge.count = count;
            bge.textureViewDimension = viewDimension;
            bge.textureSampleType = sampleType;
            return bge;
        }

        static BindGroupEntry Sampler(const std::string& name, uint32_t binding, GfxShaderStageFlags stageFlags, bool nonFiltering = false)
        {
            BindGroupEntry bge{};
            bge.name = name;
            bge.binding = binding;
            bge.bindingType = GFX_BINDING_TYPE_SAMPLER;
            bge.stageFlags = stageFlags;
            bge.samplerNonFiltering = nonFiltering;
            return bge;
        }

        static BindGroupEntry StorageTexture(const std::string& name, uint32_t binding, GfxShaderStageFlags stageFlags, GfxFormat format, GfxTextureViewType viewDimension = GFX_TEXTURE_VIEW_TYPE_2D, GfxStorageTextureAccess access = GFX_STORAGE_TEXTURE_ACCESS_WRITE_ONLY)
        {
            BindGroupEntry bge{};
            bge.name = name;
            bge.binding = binding;
            bge.bindingType = GFX_BINDING_TYPE_STORAGE_TEXTURE;
            bge.stageFlags = stageFlags;
            bge.storageTextureFormat = format;
            bge.storageTextureViewDimension = viewDimension;
            bge.storageTextureAccess = access;
            return bge;
        }
    };

public:
    ShaderBuilder(GfxDevice device);

    ~ShaderBuilder() = default;

public:
    ShaderBuilder& AddShaderStagePath(GfxShaderStageFlags stage, const std::string& path);

    ShaderBuilder& AddShaderStagePaths(const std::map<GfxShaderStageFlags, std::string>& stagePaths);

    ShaderBuilder& AddShaderStageByteCode(GfxShaderStageFlags stage, const std::vector<char>& byteCode);

    ShaderBuilder& AddShaderStageByteCodes(const std::map<GfxShaderStageFlags, std::vector<char>>& byteCodes);

    ShaderBuilder& AddVertexInputBinding(const VertexInputBinding& desc);

    ShaderBuilder& AddVertexInputBindings(const std::vector<VertexInputBinding>& descs);

    ShaderBuilder& AddVertexInputAttribute(const VertexInputAttribute& desc);

    ShaderBuilder& AddVertexInputAttributes(const std::vector<VertexInputAttribute>& descs);

    ShaderBuilder& AddBindGroupEntry(const BindGroupEntry& entry);

    ShaderBuilder& AddBindGroupEntries(const std::vector<BindGroupEntry>& entries);

    ShaderBuilder& SetBindGroupCapacity(uint32_t size);

    ShaderBuilder& SetEntryPointName(const std::string& name);

    std::unique_ptr<Shader> Build() const;

private:
    GfxShader CreateShaderModule(const std::vector<char>& code, GfxShaderSourceType sourceType) const;

    GfxBindGroupLayout CreateBindGroupLayout() const;

private:
    GfxDevice m_device;

    std::map<GfxShaderStageFlags, std::string> m_stagePaths;

    std::map<GfxShaderStageFlags, std::vector<char>> m_stageByteCodes;

    std::vector<VertexInputBinding> m_vertexInputBindings;

    std::vector<VertexInputAttribute> m_vertexInputAttributes;

    std::vector<BindGroupEntry> m_bindGroupEntries;

    uint32_t m_bindGroupCapacity{};

    std::string m_entryPointName;

private:
    static inline const std::string DEFAULT_ENTRY_POINT_NAME{ "main" };
};
} // namespace prev::render::shader

#endif