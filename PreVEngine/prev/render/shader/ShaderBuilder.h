#ifndef __SHADER_BUILDER_H__
#define __SHADER_BUILDER_H__

#include "Shader.h"

#include <map>
#include <memory>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder final {
public:
    struct DescriptorSet {
        std::string name{};
        uint32_t binding{};
        GfxBindingType bindingType{};
        GfxShaderStageFlags stageFlags{};
        bool optional{};
        uint32_t count{ 1 };
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

    ShaderBuilder& AddDescriptorSet(const DescriptorSet& descriptorSet);

    ShaderBuilder& AddDescriptorSets(const std::vector<DescriptorSet>& descriptorSets);

    ShaderBuilder& SetBindGroupCapacity(uint32_t size);

    ShaderBuilder& SetEntryPointName(const std::string& name);

    std::unique_ptr<Shader> Build() const;

private:
    GfxShader CreateShaderModule(const std::vector<char>& spirv) const;

    GfxBindGroupLayout CreateBindGroupLayout() const;

private:
    GfxDevice m_device;

    std::map<GfxShaderStageFlags, std::string> m_stagePaths;

    std::map<GfxShaderStageFlags, std::vector<char>> m_stageByteCodes;

    std::vector<VertexInputBinding> m_vertexInputBindings;

    std::vector<VertexInputAttribute> m_vertexInputAttributes;

    std::vector<DescriptorSet> m_descriptorSets;

    uint32_t m_bindGroupCapacity{};

    std::string m_entryPointName;

private:
    static inline const std::string DEFAULT_ENTRY_POINT_NAME{ "main" };
};
} // namespace prev::render::shader

#endif