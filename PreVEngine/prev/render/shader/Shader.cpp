#include "Shader.h"

#include "../../core/Formats.h"
#include "../../util/MathUtils.h"
#include "../../util/VkUtils.h"

namespace prev::render::shader {
Shader::Shader(const VkDevice device)
    : m_device{ device }
    , m_descriptorPool{ VK_NULL_HANDLE }
    , m_descriptorSetLayout{ VK_NULL_HANDLE }
    , m_poolCapacity{ 0 }
    , m_currentDescriptorSetIndex{ 0 }
{
}

Shader::~Shader()
{
    vkDeviceWaitIdle(m_device);

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

    for (uint32_t i = 0; i < size; ++i) {
        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        VKERRCHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &descriptorSet));
        m_descriptorSets[i] = descriptorSet;
    }
}

bool Shader::ShouldAdjustCapacity(const uint32_t size) const
{
    const float MIN_CAPACITY_RATIO_TO_SHRINK{ 0.5f };

    bool shouldAdjust{ false };
    if (size > m_poolCapacity) {
        shouldAdjust = true;
    } else if (float(size) / float(m_poolCapacity) < MIN_CAPACITY_RATIO_TO_SHRINK) {
        shouldAdjust = true;
    }
    return shouldAdjust;
}

bool Shader::AdjustDescriptorPoolCapacity(const uint32_t desiredCount)
{
    bool shouldRecreate{ ShouldAdjustCapacity(desiredCount) };
    if (shouldRecreate) {
        RecreateDescriptorPool(desiredCount);
        RecreateDescriptorSets(desiredCount);
        return true;
    }
    return false;
}

VkDescriptorPool Shader::CreateDescriptorPool(const uint32_t size) const
{
    const size_t itemsSize{ m_layoutBindings.size() * size };

    std::vector<VkDescriptorPoolSize> poolSizes(itemsSize);
    for (size_t i = 0; i < itemsSize; ++i) {
        const auto& binding{ m_layoutBindings[i % m_layoutBindings.size()] };

        poolSizes[i].type = binding.descriptorType;
        poolSizes[i].descriptorCount = binding.descriptorCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    poolInfo.maxSets = static_cast<uint32_t>(size);
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool descriptorPool;
    VKERRCHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &descriptorPool));
    return descriptorPool;
}

VkDescriptorSet Shader::UpdateNextDescriptorSet()
{
    CheckBindings();

    VkDescriptorSet descriptorSet = m_descriptorSets[m_currentDescriptorSetIndex];

    std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos(m_descriptorWrites.size());
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos(m_descriptorWrites.size());
    for (size_t writeIndex = 0; writeIndex < m_descriptorWrites.size(); writeIndex++) {

        auto& currentBufferInfos{ bufferInfos[writeIndex] };
        auto& currentImageInfos{ imageInfos[writeIndex] };

        for (const auto& info : m_descriptorSetInfos) {
            if (info.second.writeIndex == writeIndex) {
                currentBufferInfos.push_back(info.second.bufferInfo);
                currentImageInfos.push_back(info.second.imageInfo);
            }
        }

        auto& write{ m_descriptorWrites[writeIndex] };
        write.dstSet = descriptorSet;
        write.pBufferInfo = currentBufferInfos.data();
        write.pImageInfo = currentImageInfos.data();
    }

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);

    m_currentDescriptorSetIndex = (m_currentDescriptorSetIndex + 1) % m_poolCapacity;

    return descriptorSet;
}

void Shader::Bind(const std::string& name, const prev::render::buffer::UnifomRingBufferItem& uniformBuffer)
{
    const auto descriptorSetInfoIter{ m_descriptorSetInfos.find(name) };
    if (descriptorSetInfoIter == m_descriptorSetInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
    }

    auto& item{ descriptorSetInfoIter->second };

    item.bufferInfo.buffer = uniformBuffer;
    item.bufferInfo.offset = uniformBuffer.GetOffset();
    item.bufferInfo.range = uniformBuffer.GetRange();

    // LOGI("Bind UniformBuffer to shader-in: \"%s\"", name.c_str());
}

void Shader::Bind(const std::string& name, const prev::render::buffer::Buffer& buffer)
{
    const auto descriptorSetInfoIter{ m_descriptorSetInfos.find(name) };
    if (descriptorSetInfoIter == m_descriptorSetInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
    }

    auto& item{ descriptorSetInfoIter->second };

    item.bufferInfo.buffer = buffer;
    item.bufferInfo.offset = 0;
    item.bufferInfo.range = buffer.GetSize();

    // LOGI("Bind Buffer to shader-in: \"%s\"", name.c_str());
}

void Shader::Bind(const std::string& name, const prev::render::buffer::ImageBuffer& imageBuffer, const prev::render::sampler::Sampler& sampler, const VkImageLayout layout)
{
    const auto descriptorSetInfoIter{ m_descriptorSetInfos.find(name) };
    if (descriptorSetInfoIter == m_descriptorSetInfos.cend()) {
        LOGE("Could not find uniform with name: %s", name.c_str());
    }

    auto& item{ descriptorSetInfoIter->second };

    item.imageInfo.imageView = imageBuffer.GetImageView();
    item.imageInfo.imageLayout = layout;
    item.imageInfo.sampler = sampler;

    // LOGI("Bind Image to shader-in: \"%s\"", name.c_str());
}

void Shader::CheckBindings() const
{
    for (auto& item : m_descriptorSetInfos) {
        if (item.second.bufferInfo.buffer == 0) {
            LOGE("Shader item: \"%s\" was not bound. Set a binding before creating the DescriptorSet.", item.first.c_str());
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

const std::vector<VkVertexInputBindingDescription>& Shader::GetVertexInputBindingDescriptions() const
{
    return m_inputBindingDescriptions;
}

const std::vector<VkVertexInputAttributeDescription>& Shader::GetVertexInputAttributeDescriptions() const
{
    return m_inputAttributeDescriptions;
}

const std::vector<VkPipelineShaderStageCreateInfo>& Shader::GetShaderStages() const
{
    return m_shaderStages;
}
} // namespace prev::render::shader
