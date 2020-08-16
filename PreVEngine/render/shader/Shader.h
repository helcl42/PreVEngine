#ifndef __SHADER_H__
#define __SHADER_H__

#include <map>
#include <memory>
#include <vector>

#include "../../core/instance/Validation.h"
#include "../../core/memory/buffer/UniformBuffer.h"
#include "../../core/memory/image/IImageBuffer.h"

namespace prev {
class Shader {
private:
    struct DescriptorSetInfo {
        size_t writeIndex;

        union {
            VkDescriptorBufferInfo bufferInfo;

            VkDescriptorImageInfo imageInfo;
        };
    };

private:
    static inline const std::string DEFAULT_ENTRY_POINT_NAME{ "main" };

protected:
    VkDevice m_device;

    // Shader Stages
    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    // Descriptor Sets
    uint32_t m_poolCapacity;

    uint32_t m_currentDescriptorSetIndex;

    VkDescriptorPool m_descriptorPool;

    std::vector<VkDescriptorSet> m_descriptorSets;

    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

    std::vector<VkWriteDescriptorSet> m_descriptorWrites;

    VkDescriptorSetLayout m_descriptorSetLayout;

    std::map<std::string, DescriptorSetInfo> m_descriptorSetInfos;

    std::vector<VkPushConstantRange> m_pushConstantRanges;

    //Vertex Inputs
    std::vector<VkVertexInputBindingDescription> m_inputBindingDescriptions;

    std::vector<VkVertexInputAttributeDescription> m_inputAttributeDescriptions;

private:
    VkShaderModule CreateShaderModule(const std::vector<char>& spirv) const;

    void CheckBindings() const;

    VkDescriptorSetLayout CreateDescriptorSetLayout() const;

    VkDescriptorPool CreateDescriptorPool(const uint32_t size) const;

    void RecreateDescriptorPool(const uint32_t size);

    void RecreateDescriptorSets(const uint32_t size);

    bool ShouldAdjustCapacity(const uint32_t size);

protected:
    void AddDescriptorSet(const std::string& name, const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags);

    void AddPushConstantBlock(const VkShaderStageFlags stageFlags, const uint32_t offset, const uint32_t size);

protected:
    virtual void InitVertexInputs() = 0;

    virtual void InitDescriptorSets() = 0;

    virtual void InitPushConstantsBlocks() = 0;

public:
    Shader(const VkDevice device);

    virtual ~Shader();

public:
    bool Init();

    void ShutDown();

    bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

    bool AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv);

    void Bind(const std::string& name, const prev::core::memory::buffer::UBO& ubo);

    void Bind(const std::string& name, const prev::core::memory::buffer::Buffer& buffer);

    void Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler, const VkImageLayout imageLayout);

    void Bind(const std::string& name, const prev::core::memory::image::IImageBuffer& image, const VkImageLayout imageLayout);

    VkDescriptorSet UpdateNextDescriptorSet();

public:
    const VkDescriptorSetLayout* GetDescriptorSetLayout() const;

    const std::vector<VkPushConstantRange>& GetPushConstantsRanges() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;

    const std::vector<VkVertexInputBindingDescription>& GetVertexInputBindingDescriptions() const;

    const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const;
};

} // namespace prev

#endif
