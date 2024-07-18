#ifndef __SHADER_H__
#define __SHADER_H__

#include "../buffer/Buffer.h"
#include "../buffer/ImageBuffer.h"
#include "../buffer/UniformBuffer.h"
#include "../sampler/Sampler.h"

#include "../../core/Core.h"

#include <map>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder;

class Shader final {
public:
    Shader(const VkDevice device); // TODO - this should be private - accesible from builder only!;

    ~Shader();

public:
    bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

    void Bind(const std::string& name, const prev::render::buffer::UniformBuffer& uniformBuffer);

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
    struct DescriptorSet {
        std::string name{};
        uint32_t binding{};
        VkDescriptorType descType{};
        uint32_t descCount{};
        VkShaderStageFlags stageFlags{};
    };

    struct PushConstantBlock {
        VkShaderStageFlags stageFlags{};
        uint32_t offset{};
        uint32_t size{};
    };

    struct DescriptorSetInfo {
        size_t writeIndex{};
        union {
            VkDescriptorBufferInfo bufferInfo;
            VkDescriptorImageInfo imageInfo;
        };
    };

private:
    void CheckBindings() const;

    VkDescriptorPool CreateDescriptorPool(const uint32_t size) const;

    void RecreateDescriptorPool(const uint32_t size);

    void RecreateDescriptorSets(const uint32_t size);

    bool ShouldAdjustCapacity(const uint32_t size) const;

private:
    friend class ShaderBuilder;

private:
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
