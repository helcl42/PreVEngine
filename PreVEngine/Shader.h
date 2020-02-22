#ifndef __SHADER_H__
#define __SHADER_H__

#include <map>
#include <memory>
#include <set>

#include "Buffers.h"
#include "Window.h"

namespace PreVEngine {
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
    const std::string m_shadersEntryPointName = "main";

    const std::set<VkShaderStageFlagBits> m_validShaderStages = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        VK_SHADER_STAGE_GEOMETRY_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        VK_SHADER_STAGE_COMPUTE_BIT // TODO: should be really here??
    };

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
    VkVertexInputBindingDescription m_inputBindingDescription;

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

    ~Shader();

public:
    bool Init();

    void ShutDown();

    bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

    bool AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv);

    void Bind(const std::string& name, const UBO& ubo);

    void Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler, const VkImageLayout imageLayout);

    void Bind(const std::string& name, const IImageBuffer& image, const VkImageLayout imageLayout);

    VkDescriptorSet UpdateNextDescriptorSet();

public:
    const VkDescriptorSetLayout* GetDescriptorSetLayout() const;

    const std::vector<VkPushConstantRange>& GetPushConstantsRanges() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;

    const VkVertexInputBindingDescription* GetVertexInputBindingDescription() const;

    const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDewcriptions() const;
};

class ShaderFactory {
private:
    std::vector<char> LoadByteCodeFromFile(const std::string& filename) const;

public:
    template <typename ShaderType>
    std::shared_ptr<Shader> CreateShaderFromFiles(const VkDevice device, const std::map<VkShaderStageFlagBits, std::string>& stagePaths) const
    {
        std::map<VkShaderStageFlagBits, std::vector<char> > byteCodes;
        for (const auto& stagePath : stagePaths) {
            const auto spirv = LoadByteCodeFromFile(stagePath.second);
            byteCodes.insert(std::make_pair(stagePath.first, spirv));
        }

        return CreateShaderFromByteCodes<ShaderType>(device, byteCodes);
    }

    template <typename ShaderType>
    std::shared_ptr<Shader> CreateShaderFromByteCodes(const VkDevice device, const std::map<VkShaderStageFlagBits, std::vector<char> >& byteCodes) const
    {
        std::shared_ptr<Shader> shaders = std::make_shared<ShaderType>(device);

        for (const auto& byteCode : byteCodes) {
            shaders->AddShaderModule(byteCode.first, byteCode.second);
        }

        shaders->Init();

        return shaders;
    }
};
} // namespace PreVEngine

#endif
