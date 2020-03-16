#include "Shader.h"
#include "Formats.h"
#include "Utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace PreVEngine {
Shader::Shader(const VkDevice device)
    : m_device(device)
    , m_descriptorPool(VK_NULL_HANDLE)
    , m_descriptorSetLayout(VK_NULL_HANDLE)
    , m_poolCapacity(0)
    , m_currentDescriptorSetIndex(0)
{
}

Shader::~Shader()
{
    ShutDown();
}

bool Shader::Init()
{
    if (m_descriptorSetLayout == VK_NULL_HANDLE) {
        for (const auto& module : m_shaderModules) {
            VkPipelineShaderStageCreateInfo stageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            stageInfo.stage = module.first;
            stageInfo.module = m_shaderModules[module.first];
            stageInfo.pName = DEFAULT_ENTRY_POINT_NAME.c_str();
            m_shaderStages.push_back(stageInfo);
        }

        InitVertexInputs();
        InitDescriptorSets();
        InitPushConstantsBlocks();

        m_currentDescriptorSetIndex = 0;

        m_descriptorSetLayout = CreateDescriptorSetLayout();

        AdjustDescriptorPoolCapacity(20);

        return true;
    }

    return false;
}

void Shader::ShutDown()
{
    if (m_device) {
        vkDeviceWaitIdle(m_device);
    }

    if (m_descriptorPool) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }

    if (m_descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }

    for (auto& shaderModule : m_shaderModules) {
        vkDestroyShaderModule(m_device, shaderModule.second, nullptr);
    }
    m_shaderModules.clear();
}

void Shader::RecreateDescriptorPool(const uint32_t size)
{
    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    }

    m_descriptorPool = CreateDescriptorPool(size);
    m_poolCapacity = size;
}

void Shader::RecreateDescriptorSets(const uint32_t size)
{
    m_descriptorSets.resize(size);

    for (uint32_t i = 0; i < size; i++) {
        VkDescriptorSet descriptorSet = {};

        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_descriptorSetLayout;
        VKERRCHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &descriptorSet););

        m_descriptorSets[i] = descriptorSet;
    }
}

bool Shader::ShouldAdjustCapacity(const uint32_t size)
{
    const float MIN_CAPACITY_RATIO_TO_SHRINK = 0.5f;

    bool shouldAdjust = false;
    if (size > m_poolCapacity) {
        shouldAdjust = true;
    } else if (float(size) / float(m_poolCapacity) < MIN_CAPACITY_RATIO_TO_SHRINK) {
        shouldAdjust = true;
    }
    return shouldAdjust;
}

bool Shader::AdjustDescriptorPoolCapacity(const uint32_t desiredCount)
{
    bool shouldRecreate = ShouldAdjustCapacity(desiredCount);

    if (shouldRecreate) {
        RecreateDescriptorPool(desiredCount);
        RecreateDescriptorSets(desiredCount);

        return true;
    }

    return false;
}

void Shader::AddDescriptorSet(const std::string& name, const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags)
{
    m_layoutBindings.emplace_back(VkUtils::CreteDescriptorSetLayoutBinding(binding, descType, descCount, stageFlags));
    m_descriptorWrites.emplace_back(VkUtils::CreateWriteDescriptorSet(binding, descType, descCount));
    if (descCount > 1) {
        for (uint32_t i = 0; i < descCount; i++) {
            m_descriptorSetInfos[name + "[" + std::to_string(i) + "]"] = { m_descriptorWrites.size() - 1 };
        }
    } else {
        m_descriptorSetInfos[name] = { m_descriptorWrites.size() - 1 };
    }
}

void Shader::AddPushConstantBlock(const VkShaderStageFlags stageFlags, const uint32_t offset, const uint32_t size)
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = stageFlags;
    pushConstantRange.offset = offset;
    pushConstantRange.size = size;
    m_pushConstantRanges.emplace_back(pushConstantRange);
}

bool Shader::AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv)
{
    assert(!(VALID_SHADER_STAGES.find(stage) == VALID_SHADER_STAGES.cend()) && "Invalid shader stage provided.");

    assert(!m_shaderModules[stage] && "Shader stage already loaded.");

    m_shaderModules[stage] = CreateShaderModule(spirv);

    return !!m_shaderModules[stage];
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char>& spirv) const
{
    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = spirv.size();

    std::vector<uint32_t> codeAligned(spirv.size() / 4 + 1);
    memcpy(codeAligned.data(), spirv.data(), spirv.size());
    createInfo.pCode = codeAligned.data();

    VkShaderModule shaderModule;
    VKERRCHECK(vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule));

    return shaderModule;
}

VkDescriptorSetLayout Shader::CreateDescriptorSetLayout() const
{
    VkDescriptorSetLayout descriptorSetLayout;

    VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    createInfo.bindingCount = static_cast<uint32_t>(m_layoutBindings.size());
    createInfo.pBindings = m_layoutBindings.data();

    VKERRCHECK(vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &descriptorSetLayout));

    return descriptorSetLayout;
}

VkDescriptorPool Shader::CreateDescriptorPool(const uint32_t size) const
{
    VkDescriptorPool descriptorPool;

    const size_t itemsSize = m_layoutBindings.size() * size;

    std::vector<VkDescriptorPoolSize> poolSizes(itemsSize);
    for (size_t i = 0; i < itemsSize; ++i) {
        const auto& binding = m_layoutBindings[i % m_layoutBindings.size()];

        poolSizes[i].type = binding.descriptorType;
        poolSizes[i].descriptorCount = binding.descriptorCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    poolInfo.maxSets = static_cast<uint32_t>(size);
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    VKERRCHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &descriptorPool));

    return descriptorPool;
}

VkDescriptorSet Shader::UpdateNextDescriptorSet()
{
    CheckBindings();

    VkDescriptorSet descriptorSet = m_descriptorSets[m_currentDescriptorSetIndex];

    std::vector<std::vector<VkDescriptorBufferInfo> > bufferInfos(m_descriptorWrites.size());
    std::vector<std::vector<VkDescriptorImageInfo> > imageInfos(m_descriptorWrites.size());
    for (size_t writeIndex = 0; writeIndex < m_descriptorWrites.size(); writeIndex++) {

        auto& currentBufferInfos = bufferInfos[writeIndex];
        auto& currentImageInfos = imageInfos[writeIndex];

        for (const auto& info : m_descriptorSetInfos) {
            if (info.second.writeIndex == writeIndex) {
                currentBufferInfos.push_back(info.second.bufferInfo);
                currentImageInfos.push_back(info.second.imageInfo);
            }
        }

        auto& write = m_descriptorWrites[writeIndex];
        write.dstSet = descriptorSet;
        write.pBufferInfo = currentBufferInfos.data();
        write.pImageInfo = currentImageInfos.data();
    }

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);

    m_currentDescriptorSetIndex = (m_currentDescriptorSetIndex + 1) % m_poolCapacity;

    return descriptorSet;
}

void Shader::Bind(const std::string& name, const UBO& ubo)
{
    if (m_descriptorSetInfos.find(name) == m_descriptorSetInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
    }

    auto& item = m_descriptorSetInfos[name];

    item.bufferInfo.buffer = ubo;
    item.bufferInfo.offset = 0;
    item.bufferInfo.range = VK_WHOLE_SIZE;

    //LOGI("Bind UBO   to shader-in: \"%s\"\n", name.c_str());
}

void Shader::Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler, const VkImageLayout imageLayout)
{
    if (m_descriptorSetInfos.find(name) == m_descriptorSetInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
    }

    auto& item = m_descriptorSetInfos[name];

    item.imageInfo.imageView = imageView;
    item.imageInfo.sampler = sampler;
    item.imageInfo.imageLayout = imageLayout;

    //LOGI("Bind UBO   to shader-in: \"%s\"\n", name.c_str());
}

void Shader::Bind(const std::string& name, const IImageBuffer& image, const VkImageLayout imageLayout)
{
    Bind(name, image.GetImageView(), image.GetSampler(), imageLayout);
}

void Shader::CheckBindings() const
{
    for (auto& item : m_descriptorSetInfos) {
        if (item.second.bufferInfo.buffer == 0) {
            LOGE("Shader item: \"%s\" was not bound. Set a binding before creating the DescriptorSet.\n", item.first.c_str());
            PAUSE;
            exit(0);
        }
    }
}

const VkDescriptorSetLayout* Shader::GetDescriptorSetLayout() const
{
    return &m_descriptorSetLayout;
}

const std::vector<VkPushConstantRange>& Shader::GetPushConstantsRanges() const
{
    return m_pushConstantRanges;
}

const VkVertexInputBindingDescription* Shader::GetVertexInputBindingDescription() const
{
    return &m_inputBindingDescription;
}

const std::vector<VkVertexInputAttributeDescription>& Shader::GetVertexInputAttributeDewcriptions() const
{
    return m_inputAttributeDescriptions;
}

const std::vector<VkPipelineShaderStageCreateInfo>& Shader::GetShaderStages() const
{
    return m_shaderStages;
}

std::vector<char> ShaderFactory::LoadByteCodeFromFile(const std::string& filename) const
{
    LOGI("Load Shader: %s\n", filename.c_str());

    std::ifstream fileStream(filename, std::ios_base::binary);

    assert(fileStream.good() && "Could not open shader file.");

    fileStream.seekg(0, fileStream.end);
    size_t length = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);

    assert(length > 0 && "Could not read file content");

    std::vector<char> buffer;

    buffer.resize(length);
    fileStream.read(&buffer[0], length);

    fileStream.close();

    return buffer;
}
} // namespace PreVEngine