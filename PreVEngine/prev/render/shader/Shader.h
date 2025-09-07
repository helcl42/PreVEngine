#ifndef __SHADER_H__
#define __SHADER_H__

#include "../buffer/Buffer.h"
#include "../buffer/ImageBuffer.h"
#include "../sampler/Sampler.h"

#include "../../core/Core.h"

#include <map>
#include <vector>

namespace prev::render::shader {
class ShaderBuilder;

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

    struct ShadersInfo {
        std::map<VkShaderStageFlagBits, VkShaderModule> modules;
        std::vector<VkPipelineShaderStageCreateInfo> stages;
    };

    struct VertexInputsInfo {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    };

    struct DescriptorSetsInfo {
        VkDescriptorSetLayout layout{};
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        std::vector<VkWriteDescriptorSet> writes;
        std::map<std::string, DescriptorSetInfo> infos;
    };

private:
    Shader(const VkDevice device, const ShadersInfo& shadersInfo, const VertexInputsInfo& vertexInputsInfo, const DescriptorSetsInfo& descriptorSetsInfo, const std::vector<VkPushConstantRange>& pushConstantRanges);

public:
    ~Shader();

public:
    bool AdjustDescriptorPoolCapacity(const uint32_t size);

    void Bind(const std::string& name, const prev::render::buffer::Buffer& buffer);

    void Bind(const std::string& name, const prev::render::buffer::ImageBuffer& imageBuffer, const prev::render::sampler::Sampler& sampler, const VkImageLayout layout);

    VkDescriptorSet UpdateNextDescriptorSet();

public:
    const VkDescriptorSetLayout& GetDescriptorSetLayout() const;

    const std::vector<VkPushConstantRange>& GetPushConstantsRanges() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;

    const std::vector<VkVertexInputBindingDescription>& GetVertexInputBindingDescriptions() const;

    const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const;

public:
    friend class ShaderBuilder;

private:
    void CheckBindings() const;

    bool ShouldAdjustCapacity(const uint32_t size) const;

private:
    VkDevice m_device;

    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    ShadersInfo m_shaders;

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
