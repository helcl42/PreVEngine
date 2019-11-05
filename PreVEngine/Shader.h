#ifndef __SHADER_H__
#define __SHADER_H__

#include <map>
#include <set>
#include <memory>

#include "Window.h"
#include "Buffers.h"

#include "External/spirv_reflect.h"

namespace PreVEngine
{
	class Shader
	{
	private:
		struct DescriptorSetInfo
		{
			std::string name;

			union
			{
				VkDescriptorBufferInfo bufferInfo;

				VkDescriptorImageInfo  imageInfo;
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

	private:
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

		std::vector<DescriptorSetInfo> m_descriptorSetInfos;

		std::map<std::string, size_t> m_descriptorInfoNameToIndexMapping;

		//Vertex Inputs
		VkVertexInputBindingDescription m_inputBindingDescription;

		std::vector<VkVertexInputAttributeDescription> m_inputAttributeDescriptions;

		VkPipelineVertexInputStateCreateInfo m_vertexInputState;

	private:
		static void PrintModuleInfo(const SpvReflectShaderModule& module);

		static void PrintDescriptorSet(const SpvReflectDescriptorSet& set);

		static std::string ToStringDescriptorType(const SpvReflectDescriptorType& value);

		static std::string ToStringGLSLType(const SpvReflectTypeDescription& type);

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& spirv) const;

		void Parse(const std::vector<char>& spirv);

		void ParseInputs(SpvReflectShaderModule& module);

		void CheckBindings() const;

		VkDescriptorSetLayout CreateDescriptorSetLayout() const;

		VkDescriptorPool CreateDescriptorPool(const uint32_t size) const;

		void RecreateDescriptorPool(const uint32_t size);

		void RecreateDescriptorSets(const uint32_t size);

		bool ShouldAdjustCapacity(const uint32_t size);

	public:
		Shader(VkDevice device);

		~Shader();

	public:
		bool Init();

		void ShutDown();

		bool AdjustDescriptorPoolCapacity(const uint32_t desiredCount);

		bool AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv);

		void Bind(const std::string& name, const UBO& ubo);

		void Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler);

		void Bind(const std::string& name, const ImageBuffer& image);

		VkDescriptorSet UpdateNextDescriptorSet();

	public:
		const VkDescriptorSetLayout* GetDescriptorSetLayout() const;

		const VkPipelineVertexInputStateCreateInfo* GetVertextInputState() const;

		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const;
	};

	class ShaderFactory
	{
	private:
		std::vector<char> LoadByteCodeFromFile(const std::string& filename) const;

	public:
		std::shared_ptr<Shader> CreateShaderFromFiles(VkDevice device, const std::map<VkShaderStageFlagBits, std::string>& stagePaths) const;

		std::shared_ptr<Shader> CreateShaderFromByteCodes(VkDevice device, const std::map<VkShaderStageFlagBits, std::vector<char>>& byteCodes) const;
	};
}

#endif
