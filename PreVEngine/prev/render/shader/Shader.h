#ifndef __SHADER_H__
#define __SHADER_H__

#include "../buffer/Buffer.h"
#include "../buffer/ImageBuffer.h"
#include "../sampler/Sampler.h"

#include "../../core/Core.h"

#include <map>
#include <vector>

namespace prev::render::shader {
class Shader final {
public:
    struct DescriptorSet {
        std::string name{};
        uint32_t binding{};
        VkDescriptorType descType{};
        uint32_t descCount{};
        VkShaderStageFlags stageFlags{};
    };

    struct DescriptorSetInfo {
        size_t writeIndex{};
        union {
            VkDescriptorBufferInfo bufferInfo;
            VkDescriptorImageInfo imageInfo;
        };
    };

public:
    Shader(
        const VkDevice device,
        const std::map<VkShaderStageFlagBits, VkShaderModule>& shaderModules,
        const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
        const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions,
        const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions,
        const VkDescriptorSetLayout descriptorSetLayout,
        const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings,
        const std::vector<VkWriteDescriptorSet>& descriptorWrites,
        const std::map<std::string, DescriptorSetInfo>& descriptorSetInfos,
        const std::vector<VkPushConstantRange>& pushConstantRanges);

    ~Shader();

public:
    bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

    void Bind(const std::string& name, const prev::render::buffer::Buffer& buffer);

    void Bind(const std::string& name, const prev::render::buffer::ImageBuffer& imageBuffer, const prev::render::sampler::Sampler& sampler, const VkImageLayout layout);

    VkDescriptorSet UpdateNextDescriptorSet();

public:
    const VkDescriptorSetLayout* GetDescriptorSetLayout() const;

    const std::vector<VkPushConstantRange>& GetPushConstantsRanges() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;

    const std::vector<VkVertexInputBindingDescription>& GetVertexInputBindingDescriptions() const;

    const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const;

private:
    void CheckBindings() const;

    VkDescriptorPool CreateDescriptorPool(const uint32_t size) const;

    void RecreateDescriptorPool(const uint32_t size);

    void RecreateDescriptorSets(const uint32_t size);

    bool ShouldAdjustCapacity(const uint32_t size) const;

private:
    VkDevice m_device;

    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    std::vector<VkVertexInputBindingDescription> m_vertexInputBindingDescriptions;

    std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeDescriptions;

    VkDescriptorSetLayout m_descriptorSetLayout;

    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

    std::vector<VkWriteDescriptorSet> m_descriptorWrites;

    std::map<std::string, DescriptorSetInfo> m_descriptorSetInfos;

    std::vector<VkPushConstantRange> m_pushConstantRanges;

    VkDescriptorPool m_descriptorPool;

    uint32_t m_poolCapacity;

    uint32_t m_currentDescriptorSetIndex;

    std::vector<VkDescriptorSet> m_descriptorSets;
};

} // namespace prev::render::shader

#endif
