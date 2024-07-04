#include "ShaderBuilder.h"

#include "../../util/Utils.h"
#include "../../util/VkUtils.h"

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

ShaderBuilder& ShaderBuilder::AddPushConstantBlock(const Shader::PushConstantBlock& pushConstantBlock)
{
    m_pushConstantBlocks.push_back(pushConstantBlock);
    return *this;
}

ShaderBuilder& ShaderBuilder::AddPushConstantBlocks(const std::vector<Shader::PushConstantBlock>& pushConstantBlocks)
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

std::unique_ptr<Shader> ShaderBuilder::Build() const
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

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::map<std::string, Shader::DescriptorSetInfo> descriptorSetInfos;
    for (const auto& ds : m_descriptorSets) {
        layoutBindings.emplace_back(prev::util::vk::CreteDescriptorSetLayoutBinding(ds.binding, ds.descType, ds.descCount, ds.stageFlags));
        descriptorWrites.emplace_back(prev::util::vk::CreateWriteDescriptorSet(ds.binding, ds.descType, ds.descCount));
        if (ds.descCount > 1) {
            for (uint32_t i = 0; i < ds.descCount; ++i) {
                descriptorSetInfos[ds.name + "[" + std::to_string(i) + "]"] = { descriptorWrites.size() - 1 };
            }
        } else {
            descriptorSetInfos[ds.name] = { descriptorWrites.size() - 1 };
        }
    }

    std::vector<VkPushConstantRange> pushConstantRanges;
    for (const auto& pcb : m_pushConstantBlocks) {
        const auto pushConstantRange{ CreatePushConstantRange(pcb.stageFlags, pcb.offset, pcb.size) };
        pushConstantRanges.emplace_back(pushConstantRange);
    }

    const auto descriptorSetLayout{ CreateDescriptorSetLayout(layoutBindings) };

    auto shader{ std::make_unique<Shader>(m_device) };
    shader->m_shaderModules = shaderModules;
    shader->m_shaderStages = shaderStages;
    shader->m_inputBindingDescriptions = m_vertexInputBindingDescriptors;
    shader->m_inputAttributeDescriptions = m_vertexInputAttributeDescriptions;
    shader->m_layoutBindings = layoutBindings;
    shader->m_descriptorWrites = descriptorWrites;
    shader->m_descriptorSetInfos = descriptorSetInfos;
    shader->m_descriptorSetLayout = descriptorSetLayout;
    shader->m_pushConstantRanges = pushConstantRanges;
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
    std::vector<uint32_t> codeAligned(spirv.size() / 4 + 1);
    memcpy(codeAligned.data(), spirv.data(), spirv.size());

    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = spirv.size();
    createInfo.pCode = codeAligned.data();

    VkShaderModule shaderModule;
    VKERRCHECK(vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule));
    return shaderModule;
}

VkPipelineShaderStageCreateInfo ShaderBuilder::CreateShaderStageCreateInfo(const VkShaderStageFlagBits stage, const VkShaderModule& module) const
{
    VkPipelineShaderStageCreateInfo stageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stageInfo.stage = stage;
    stageInfo.module = module;
    stageInfo.pName = Shader::DEFAULT_ENTRY_POINT_NAME.c_str();
    return stageInfo;
}

VkDescriptorSetLayout ShaderBuilder::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings) const
{
    VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    VKERRCHECK(vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &descriptorSetLayout));
    return descriptorSetLayout;
}

VkPushConstantRange ShaderBuilder::CreatePushConstantRange(const VkShaderStageFlags stageFlags, const uint32_t offset, const uint32_t size) const
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = stageFlags;
    pushConstantRange.offset = offset;
    pushConstantRange.size = size;
    return pushConstantRange;
}

} // namespace prev::render::shader