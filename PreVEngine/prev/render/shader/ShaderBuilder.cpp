#include "ShaderBuilder.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"
#include "../../util/VkUtils.h"

#include <stdexcept>

namespace prev::render::shader {
ShaderBuilder::ShaderBuilder(VkDevice device)
    : m_device{ device }
{
}

ShaderBuilder& ShaderBuilder::AddShaderStagePath(const VkShaderStageFlagBits stage, const std::string& path)
{
    CheckExistingStage(stage);

    m_stagePaths.insert({ stage, path });
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStagePaths(const std::map<VkShaderStageFlagBits, std::string>& stagePaths)
{
    for (const auto& [stage, path] : stagePaths) {
        AddShaderStagePath(stage, path);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStageByteCode(const VkShaderStageFlagBits stage, const std::vector<char>& byteCode)
{
    CheckExistingStage(stage);

    m_stageByteCodes.insert({ stage, byteCode });
    return *this;
}

ShaderBuilder& ShaderBuilder::AddShaderStageByteCodes(const std::map<VkShaderStageFlagBits, std::vector<char>>& byteCodes)
{
    for (const auto& [stage, byteCode] : byteCodes) {
        AddShaderStageByteCode(stage, byteCode);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputBindingDescription(const VkVertexInputBindingDescription& vertexBindingDescriptor)
{
    m_vertexInputBindingDescriptors.push_back(vertexBindingDescriptor);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions)
{
    for (const auto& vertexBindingDescription : vertexBindingDescriptions) {
        AddVertexInputBindingDescription(vertexBindingDescription);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputAttributeDescription(const VkVertexInputAttributeDescription& vertexAttributeDescription)
{
    m_vertexInputAttributeDescriptions.push_back(vertexAttributeDescription);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddVertexInputAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions)
{
    for (const auto& vertexAttributeDescription : vertexAttributeDescriptions) {
        AddVertexInputAttributeDescription(vertexAttributeDescription);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddDescriptorSet(const Shader::DescriptorSet& descriptorSet)
{
    m_descriptorSets.push_back(descriptorSet);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddDescriptorSets(const std::vector<Shader::DescriptorSet>& descriptorSets)
{
    for (const auto& descriptorSet : descriptorSets) {
        AddDescriptorSet(descriptorSet);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::AddPushConstantBlock(const VkPushConstantRange& pushConstantBlock)
{
    m_pushConstantBlocks.push_back(pushConstantBlock);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddPushConstantBlocks(const std::vector<VkPushConstantRange>& pushConstantBlocks)
{
    for (const auto& pushConstantBlock : pushConstantBlocks) {
        AddPushConstantBlock(pushConstantBlock);
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::SetDescriptorPoolCapacity(const uint32_t size)
{
    m_descriptorPoolSize = size;
    return *this;
}

ShaderBuilder& ShaderBuilder::SetEntryPointName(const std::string& name)
{
    m_entryPointName = name;
    return *this;
}

std::unique_ptr<Shader> ShaderBuilder::Build() const
{
    const Shader::ShadersInfo shadersInfo{ CreateShadersInfo() };
    const Shader::VertexInputsInfo vertexInputsInfo{ CreateVertexInputsInfo() };
    const Shader::DescriptorSetsInfo descriptorSetsInfo{ CreateDescriptorSetsInfo() };

    auto shader{ std::unique_ptr<Shader>(new Shader(m_device, shadersInfo, vertexInputsInfo, descriptorSetsInfo, m_pushConstantBlocks)) };
    if (m_descriptorPoolSize > 0) {
        shader->AdjustDescriptorPoolCapacity(m_descriptorPoolSize);
    }
    return shader;
}

void ShaderBuilder::CheckExistingStage(const VkShaderStageFlagBits stage) const
{
    const auto pathIter{ m_stagePaths.find(stage) };
    if (pathIter != m_stagePaths.cend()) {
        throw std::runtime_error("Shader stage " + std::to_string(pathIter->first) + " is already present with path: " + pathIter->second + ".");
    }

    const auto byteCodeIter{ m_stageByteCodes.find(stage) };
    if (byteCodeIter != m_stageByteCodes.cend()) {
        throw std::runtime_error("Shader stage " + std::to_string(pathIter->first) + " is already present with byte code.");
    }
}

VkShaderModule ShaderBuilder::CreateShaderModule(const std::vector<char>& spirv) const
{
    std::vector<uint32_t> codeAligned(prev::util::math::RoundUp<size_t>(spirv.size() / 4, 4));
    memcpy(codeAligned.data(), spirv.data(), spirv.size());

    VkShaderModuleCreateInfo createInfo{ prev::util::vk::CreateStruct<VkShaderModuleCreateInfo>(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO) };
    createInfo.codeSize = spirv.size();
    createInfo.pCode = codeAligned.data();

    VkShaderModule shaderModule;
    VKERRCHECK(vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule));
    return shaderModule;
}

VkPipelineShaderStageCreateInfo ShaderBuilder::CreateShaderStageCreateInfo(const VkShaderStageFlagBits stage, const VkShaderModule& module) const
{
    VkPipelineShaderStageCreateInfo stageInfo{ prev::util::vk::CreateStruct<VkPipelineShaderStageCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO) };
    stageInfo.stage = stage;
    stageInfo.module = module;
    stageInfo.pName = m_entryPointName.empty() ? DEFAULT_ENTRY_POINT_NAME.c_str() : m_entryPointName.c_str(); // this changes name for all stages
    return stageInfo;
}

VkDescriptorSetLayout ShaderBuilder::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings) const
{
    VkDescriptorSetLayoutCreateInfo createInfo{ prev::util::vk::CreateStruct<VkDescriptorSetLayoutCreateInfo>(VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO) };
    createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    VKERRCHECK(vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &descriptorSetLayout));
    return descriptorSetLayout;
}

Shader::ShadersInfo ShaderBuilder::CreateShadersInfo() const
{
    auto byteCodes{ m_stageByteCodes };
    for (const auto& [stage, path] : m_stagePaths) {
        const auto spirv{ prev::util::file::ReadBinaryFile(path) };
        byteCodes.insert({ stage, spirv });
    }

    std::map<VkShaderStageFlagBits, VkShaderModule> shaderModules;
    for (const auto& [stage, byteCode] : byteCodes) {
        shaderModules.insert({ stage, CreateShaderModule(byteCode) });
    }

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (const auto& [stage, module] : shaderModules) {
        const auto stageCreateInfo{ CreateShaderStageCreateInfo(stage, module) };
        shaderStages.emplace_back(stageCreateInfo);
    }

    return { shaderModules, shaderStages };
}

Shader::VertexInputsInfo ShaderBuilder::CreateVertexInputsInfo() const
{
    return { m_vertexInputBindingDescriptors, m_vertexInputAttributeDescriptions };
}

Shader::DescriptorSetsInfo ShaderBuilder::CreateDescriptorSetsInfo() const
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::map<std::string, Shader::DescriptorSetInfo> descriptorSetInfos;
    for (const auto& ds : m_descriptorSets) {
        layoutBindings.emplace_back(prev::util::vk::CreteDescriptorSetLayoutBinding(ds.binding, ds.descType, ds.descCount, ds.stageFlags));
        descriptorWrites.emplace_back(prev::util::vk::CreateWriteDescriptorSet(ds.binding, ds.descType, ds.descCount));
        if (ds.descCount > 1) {
            for (uint32_t i = 0; i < ds.descCount; ++i) {
                descriptorSetInfos[ds.name + "[" + std::to_string(i) + "]"] = { descriptorWrites.size() - 1, {} };
            }
        } else {
            descriptorSetInfos[ds.name] = { descriptorWrites.size() - 1, {} };
        }
    }

    const auto descriptorSetLayout{ CreateDescriptorSetLayout(layoutBindings) };
    return { descriptorSetLayout, layoutBindings, descriptorWrites, descriptorSetInfos };
}

} // namespace prev::render::shader