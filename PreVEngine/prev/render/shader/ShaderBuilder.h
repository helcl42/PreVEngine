#ifndef __SHADER_BUILDER_H__
#define __SHADER_BUILDER_H__

#include "Shader.h"

#include <map>
#include <memory>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder final {
public:
    ShaderBuilder(VkDevice device);

    ~ShaderBuilder() = default;

public:
    ShaderBuilder& AddShaderStagePath(const VkShaderStageFlagBits stage, const std::string& path);

    ShaderBuilder& AddShaderStagePaths(const std::map<VkShaderStageFlagBits, std::string>& stagePaths);

    ShaderBuilder& AddShaderStageByteCode(const VkShaderStageFlagBits stage, const std::vector<char>& byteCode);

    ShaderBuilder& AddShaderStageByteCodes(const std::map<VkShaderStageFlagBits, std::vector<char>>& byteCodes);

    ShaderBuilder& AddVertexInputBindingDescription(const VkVertexInputBindingDescription& vertexBindingDescriptor);

    ShaderBuilder& AddVertexInputBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions);

    ShaderBuilder& AddVertexInputAttributeDescription(const VkVertexInputAttributeDescription& vertexAttributeDescription);

    ShaderBuilder& AddVertexInputAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions);

    ShaderBuilder& AddDescriptorSet(const Shader::DescriptorSet& descriptorSet);

    ShaderBuilder& AddDescriptorSets(const std::vector<Shader::DescriptorSet>& descriptorSets);

    ShaderBuilder& AddPushConstantBlock(const Shader::PushConstantBlock& pushConstantBlock);

    ShaderBuilder& AddPushConstantBlocks(const std::vector<Shader::PushConstantBlock>& pushConstantBlocks);

    ShaderBuilder& SetDescriptorPoolCapacity(const uint32_t size);

    ShaderBuilder& SetEntryPointName(const std::string& name);

    std::unique_ptr<Shader> Build() const;

private:
    void CheckExistingStage(const VkShaderStageFlagBits stage) const;

    VkShaderModule CreateShaderModule(const std::vector<char>& spirv) const;

    VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(const VkShaderStageFlagBits stage, const VkShaderModule& module) const;

    VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings) const;

    VkPushConstantRange CreatePushConstantRange(const VkShaderStageFlags stageFlags, const uint32_t offset, const uint32_t size) const;

private:
    VkDevice m_device;

    std::map<VkShaderStageFlagBits, std::string> m_stagePaths;

    std::map<VkShaderStageFlagBits, std::vector<char>> m_stageByteCodes;

    std::vector<VkVertexInputBindingDescription> m_vertexInputBindingDescriptors;

    std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeDescriptions;

    std::vector<Shader::DescriptorSet> m_descriptorSets;

    std::vector<Shader::PushConstantBlock> m_pushConstantBlocks;

    uint32_t m_descriptorPoolSize{};

    std::string m_entryPointName;

private:
    static inline const std::string DEFAULT_ENTRY_POINT_NAME{ "main" };
};
} // namespace prev::render::shader

#endif