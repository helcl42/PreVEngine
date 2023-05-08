#ifndef __SHADER_H__
#define __SHADER_H__

#include "../buffer/UniformBuffer.h"

#include "../../core/instance/Validation.h"

#include <map>
#include <memory>
#include <vector>

namespace prev::render::shader {
class Shader {
public:
    Shader(const VkDevice device);

    virtual ~Shader();

public:
    bool Init();

    void ShutDown();

    bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

    bool AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv);

    void Bind(const std::string& name, const prev::render::buffer::UniformBuffer& ubo);

    void Bind(const std::string& name, const prev::render::buffer::Buffer& buffer);

    void Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler, const VkImageLayout imageLayout);

    VkDescriptorSet UpdateNextDescriptorSet();

public:
    const VkDescriptorSetLayout* GetDescriptorSetLayout() const;

    const std::vector<VkPushConstantRange>& GetPushConstantsRanges() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;

    const std::vector<VkVertexInputBindingDescription>& GetVertexInputBindingDescriptions() const;

    const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const;

protected:
    struct DescriptorSet {
        std::string name;
        uint32_t binding;
        VkDescriptorType descType;
        uint32_t descCount;
        VkShaderStageFlags stageFlags;
    };

    struct PushConstantBlock {
        VkShaderStageFlags stageFlags;
        uint32_t offset;
        uint32_t size;
    };

protected:
    virtual std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptors() const = 0;

    virtual std::vector<VkVertexInputAttributeDescription> CreateInputAttributeDescriptors() const = 0;

    virtual std::vector<DescriptorSet> CreateDescriptorSets() const = 0;

    virtual std::vector<PushConstantBlock> CreatePushConstantBlocks() const = 0;

private:
    struct DescriptorSetInfo {
        size_t writeIndex;
        union {
            VkDescriptorBufferInfo bufferInfo;
            VkDescriptorImageInfo imageInfo;
        };
    };

private:
    void AddDescriptorSet(const std::string& name, const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags);

    void AddPushConstantBlock(const VkShaderStageFlags stageFlags, const uint32_t offset, const uint32_t size);

    VkShaderModule CreateShaderModule(const std::vector<char>& spirv) const;

    void CheckBindings() const;

    VkDescriptorSetLayout CreateDescriptorSetLayout() const;

    VkDescriptorPool CreateDescriptorPool(const uint32_t size) const;

    void RecreateDescriptorPool(const uint32_t size);

    void RecreateDescriptorSets(const uint32_t size);

    bool ShouldAdjustCapacity(const uint32_t size);

protected:
    static inline const std::string DEFAULT_ENTRY_POINT_NAME{ "main" };

protected:
    VkDevice m_device;

    VkDescriptorPool m_descriptorPool;

    VkDescriptorSetLayout m_descriptorSetLayout;

    uint32_t m_poolCapacity;

    uint32_t m_currentDescriptorSetIndex;

    // Shader Stages
    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    // Descriptor Sets
    std::vector<VkDescriptorSet> m_descriptorSets;

    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

    std::vector<VkWriteDescriptorSet> m_descriptorWrites;

    std::map<std::string, DescriptorSetInfo> m_descriptorSetInfos;

    std::vector<VkPushConstantRange> m_pushConstantRanges;

    // Vertex Inputs
    std::vector<VkVertexInputBindingDescription> m_inputBindingDescriptions;

    std::vector<VkVertexInputAttributeDescription> m_inputAttributeDescriptions;
};

} // namespace prev::render::shader

#endif
