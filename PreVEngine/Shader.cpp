#include "Shader.h"
#include "Formats.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace PreVEngine
{
	Shader::Shader(VkDevice device)
		: m_device(device), m_descriptorPool(VK_NULL_HANDLE), m_descriptorSetLayout(VK_NULL_HANDLE), m_poolCapacity(0)
	{
	}

	Shader::~Shader()
	{
		ShutDown();
	}

	bool Shader::Init()
	{
		if (m_descriptorSetLayout == VK_NULL_HANDLE)
		{
			m_currentDescriptorSetIndex = 0;

			m_descriptorSetLayout = CreateDescriptorSetLayout();

			AdjustDescriptorPoolCapacity(20);
		
			return true;
		}

		return false;
	}

	void Shader::ShutDown()
	{
		if (m_device)
		{
			vkDeviceWaitIdle(m_device);
		}

		if (m_descriptorPool)
		{
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if (m_descriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}

		for (auto& shaderModule : m_shaderModules)
		{
			vkDestroyShaderModule(m_device, shaderModule.second, nullptr);
		}
		m_shaderModules.clear();
	}

	void Shader::RecreateDescriptorPool(const uint32_t size)
	{
		if (m_descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
		}

		m_descriptorPool = CreateDescriptorPool(size);
		m_poolCapacity = size;
	}

	void Shader::RecreateDescriptorSets(const uint32_t size)
	{
		m_descriptorSets.resize(size);

		for (uint32_t i = 0; i < size; i++)
		{
			VkDescriptorSet descriptorSet = {};

			VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			allocInfo.descriptorPool = m_descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_descriptorSetLayout;
			VKERRCHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &descriptorSet); );

			m_descriptorSets[i] = descriptorSet;
		}
	}

	bool Shader::ShouldAdjustCapacity(const uint32_t size)
	{
		const float MIN_CAPACITY_RATIO_TO_SHRINK = 0.5f;

		bool shouldAdjust = false;
		if (size > m_poolCapacity)
		{
			shouldAdjust = true;
		}
		else if (float(size) / float(m_poolCapacity) < MIN_CAPACITY_RATIO_TO_SHRINK)
		{
			shouldAdjust = true;
		}
		return shouldAdjust;
	}

	bool Shader::AdjustDescriptorPoolCapacity(const uint32_t desiredCount)
	{
		bool shouldRecreate = ShouldAdjustCapacity(desiredCount);

		if (shouldRecreate)
		{
			RecreateDescriptorPool(desiredCount);
			RecreateDescriptorSets(desiredCount);

			return true;
		}

		return false;
	}

	bool Shader::AddShaderModule(const VkShaderStageFlagBits stage, const std::vector<char>& spirv)
	{
		assert(!(m_validShaderStages.find(stage) == m_validShaderStages.cend()) && "Invalid shader stage provided.");

		assert(!m_shaderModules[stage] && "Shader stage already loaded.");

		m_shaderModules[stage] = CreateShaderModule(spirv);

		Parse(spirv);

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

	void Shader::Parse(const std::vector<char>& spirv)
	{
		SpvReflectResult result;

		SpvReflectShaderModule module = {};
		result = spvReflectCreateShaderModule(spirv.size(), spirv.data(), &module);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		uint32_t count = 0;
		result = spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectDescriptorSet*> sets(count);
		result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		uint32_t pushConstantsCount = 0;
		result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantsCount, nullptr);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantsCount);
		result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantsCount, pushConstants.data());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		VkShaderStageFlagBits stage = (VkShaderStageFlagBits)module.shader_stage;

		for (const auto& set : sets)
		{
			for (uint32_t j = 0; j < set->binding_count; ++j)
			{
				const SpvReflectDescriptorBinding* setBinding = set->bindings[j];

				VkDescriptorSetLayoutBinding layoutBinding = {};
				layoutBinding.binding = setBinding->binding;
				layoutBinding.descriptorType = (VkDescriptorType)setBinding->descriptor_type;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = stage;
				layoutBinding.pImmutableSamplers = nullptr; // Optional
				for (uint32_t dimensionIndex = 0; dimensionIndex < setBinding->array.dims_count; ++dimensionIndex)
				{
					layoutBinding.descriptorCount *= setBinding->array.dims[dimensionIndex];
				}

				m_layoutBindings.emplace_back(layoutBinding);
			}
		}

		for (const auto& set : sets)
		{
			for (uint32_t i = 0; i < set->binding_count; ++i)
			{
				const SpvReflectDescriptorBinding* binding = set->bindings[i];
				VkDescriptorType type = (VkDescriptorType)binding->descriptor_type;

				m_descriptorSetInfos.push_back({ binding->name });

				VkWriteDescriptorSet writeDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				writeDescriptorSet.dstBinding = binding->binding;
				writeDescriptorSet.dstArrayElement = 0;
				writeDescriptorSet.descriptorType = type;
				writeDescriptorSet.descriptorCount = 1;

				m_descriptorWrites.emplace_back(writeDescriptorSet);

				m_descriptorInfoNameToIndexMapping[binding->name] = m_descriptorWrites.size() - 1;
			}
		}

		for (const auto& constBlock : pushConstants)
		{
			VkPushConstantRange pushConstantRange = {};
			pushConstantRange.stageFlags = stage;
			pushConstantRange.offset = constBlock->offset;
			pushConstantRange.size = constBlock->size;
			m_pushConstantRanges.emplace_back(pushConstantRange);
		}

		VkPipelineShaderStageCreateInfo stageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		stageInfo.stage = stage;
		stageInfo.module = m_shaderModules[stage];
		stageInfo.pName = m_shadersEntryPointName.c_str();

		m_shaderStages.push_back(stageInfo);

#ifdef ENABLE_LOGGING
		PrintModuleInfo(module);
		for (auto& set : sets)
		{
			PrintDescriptorSet(*set);
		}

		for (auto& constBlock : pushConstants)
		{
			PrintPushConstBlock(*constBlock);
		}
#endif

		if (stage == VK_SHADER_STAGE_VERTEX_BIT)
		{
			ParseInputs(module);
		}

		spvReflectDestroyShaderModule(&module);
	}

	void Shader::ParseInputs(SpvReflectShaderModule& module)
	{
		SpvReflectResult result;

		uint32_t count = 0;
		result = spvReflectEnumerateInputVariables(&module, &count, nullptr);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectInterfaceVariable*> inputVars(count);
		result = spvReflectEnumerateInputVariables(&module, &count, inputVars.data());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		// Sort attributes by location
		std::sort(std::begin(inputVars), std::end(inputVars), [](const SpvReflectInterfaceVariable* a, const SpvReflectInterfaceVariable* b) { return a->location < b->location; });

		m_inputBindingDescription = {};
		m_inputBindingDescription.binding = 0;
		m_inputBindingDescription.stride = 0;  // computed below
		m_inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		m_inputAttributeDescriptions.resize(inputVars.size());
		for (size_t varIndex = 0; varIndex < inputVars.size(); ++varIndex)
		{
			const SpvReflectInterfaceVariable* reflVar = inputVars[varIndex];

			VkVertexInputAttributeDescription& attrDescription = m_inputAttributeDescriptions[varIndex];
			attrDescription.location = reflVar->location;
			attrDescription.binding = m_inputBindingDescription.binding;
			attrDescription.format = static_cast<VkFormat>(reflVar->format);
			attrDescription.offset = 0;  // final offset computed below after sorting.
		}

		// Compute final offsets of each attribute, and total vertex stride.
		for (auto& attribute : m_inputAttributeDescriptions)
		{
			uint32_t formatSize = FormatSize(attribute.format);
			attribute.offset = m_inputBindingDescription.stride;
			m_inputBindingDescription.stride += formatSize;
		}

		m_vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		m_vertexInputState.vertexBindingDescriptionCount = 1;
		m_vertexInputState.pVertexBindingDescriptions = &m_inputBindingDescription;
		m_vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_inputAttributeDescriptions.size());
		m_vertexInputState.pVertexAttributeDescriptions = m_inputAttributeDescriptions.data();

#ifdef ENABLE_LOGGING
		printf("  Vertex Input attributes:\n");
		for (auto& var : inputVars)
		{
			printf("    %d : %s %s\n", var->location, ToStringGLSLType(*var->type_description).c_str(), var->name);
		}
		printf("\n");
#endif
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
		for (size_t i = 0; i < itemsSize; ++i)
		{
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

		uint32_t descriptorSetsCount = static_cast<uint32_t>(m_descriptorWrites.size());
		for (uint32_t writeIndex = 0; writeIndex < descriptorSetsCount; ++writeIndex)
		{
			auto& write = m_descriptorWrites[writeIndex];

			write.dstSet = descriptorSet;
			write.pBufferInfo = &m_descriptorSetInfos[writeIndex].bufferInfo;
			write.pImageInfo = &m_descriptorSetInfos[writeIndex].imageInfo;
		}

		vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(m_descriptorWrites.size()), m_descriptorWrites.data(), 0, nullptr);

		m_currentDescriptorSetIndex = (m_currentDescriptorSetIndex + 1) % m_poolCapacity;

		return descriptorSet;
	}


	void Shader::Bind(const std::string& name, const UBO& ubo)
	{
		const auto& nameIndex = m_descriptorInfoNameToIndexMapping.find(name);
		if (nameIndex == m_descriptorInfoNameToIndexMapping.cend())
		{
			LOGE("Could not find uniform with name: %s", name.c_str());
		}

		auto& item = m_descriptorSetInfos[(*nameIndex).second];

		item.bufferInfo.buffer = ubo;
		item.bufferInfo.offset = 0;
		item.bufferInfo.range = VK_WHOLE_SIZE;

		//LOGI("Bind UBO   to shader-in: \"%s\"\n", name.c_str());
	}

	void Shader::Bind(const std::string& name, const VkImageView imageView, const VkSampler sampler, const VkImageLayout imageLayout)
	{
		const auto& nameIndex = m_descriptorInfoNameToIndexMapping.find(name);
		if (nameIndex == m_descriptorInfoNameToIndexMapping.cend())
		{
			LOGE("Could not find uniform with name: %s", name.c_str());
		}

		auto& item = m_descriptorSetInfos[(*nameIndex).second];

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
		for (auto& item : m_descriptorSetInfos)
		{
			if (item.bufferInfo.buffer == 0)
			{
				LOGE("Shader item: \"%s\" was not bound. Set a binding before creating the DescriptorSet.\n", item.name.c_str());
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

	const VkPipelineVertexInputStateCreateInfo* Shader::GetVertextInputState() const
	{
		return &m_vertexInputState;
	}

	const std::vector<VkPipelineShaderStageCreateInfo>& Shader::GetShaderStages() const
	{
		return m_shaderStages;
	}

	void Shader::PrintModuleInfo(const SpvReflectShaderModule& module)
	{
		printf("  Source language : %s\n", spvReflectSourceLanguage(module.source_language));
		printf("  Entry Point     : %s\n", module.entry_point_name);

		const char* stage = "";
		switch (module.shader_stage)
		{
			case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
				stage = "VERTEX";
				break;
			case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				stage = "TESSELLATION_CONTROL";
				break;
			case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				stage = "TESSELLATION_EVALUATION";
				break;
			case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
				stage = "GEOMETRY";
				break;
			case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
				stage = "FRAGMENT";
				break;
			case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
				stage = "COMPUTE";
				break;
			default:
				stage = "UNKNOWN";
				break;
		}
		printf("  Shader stage    : %s\n", stage);
	}

	void Shader::PrintDescriptorSet(const SpvReflectDescriptorSet& set)
	{
		printf("  Descriptor set  : %d\n", set.set);
		for (uint32_t i = 0; i < set.binding_count; ++i)
		{
			const SpvReflectDescriptorBinding& binding = *set.bindings[i];
			printf("         binding  : %d\n", binding.binding);
			printf("         name     : %s\n", binding.name);
			printf("         type     : %s\n", ToStringDescriptorType(binding.descriptor_type).c_str());
		}
		printf("\n");
	}

	void Shader::PrintPushConstBlock(const SpvReflectBlockVariable& constBlock)
	{
		printf("  Const block  : name: %s - size: %d\n", constBlock.name, constBlock.padded_size);
		for (uint32_t i = 0; i < constBlock.member_count; ++i)
		{
			const SpvReflectBlockVariable& member = constBlock.members[i];
			printf("         name     : %s\n", member.name);
			printf("         offset   : %d\n", member.offset);
		}
		printf("\n");
	}

	std::string Shader::ToStringDescriptorType(const SpvReflectDescriptorType& value)
	{
		switch (value)
		{
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
				return "VK_DESCRIPTOR_TYPE_SAMPLER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
				return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
				return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
				return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
				return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
			case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
				return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
			default:
				return "VK_DESCRIPTOR_TYPE_???";
		}
	}

	std::string Shader::ToStringGLSLType(const SpvReflectTypeDescription& type)
	{
		switch (type.op)
		{
			case SpvOpTypeVector:
			{
				switch (type.traits.numeric.scalar.width)
				{
					case 32:
					{
						switch (type.traits.numeric.vector.component_count)
						{
							case 2: return "vec2";
							case 3: return "vec3";
							case 4: return "vec4";
						}
					}
					break;
					case 64:
					{
						switch (type.traits.numeric.vector.component_count)
						{
							case 2: return "dvec2";
							case 3: return "dvec3";
							case 4: return "dvec4";
						}
					}
					break;
				}
			}
			break;

			default:
				break;
		}
		return "UNKNOWN TYPE";
	}


	std::vector<char> ShaderFactory::LoadByteCodeFromFile(const std::string& filename) const
	{
		printf("Load Shader: %s... ", filename.c_str());

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

	std::shared_ptr<Shader> ShaderFactory::CreateShaderFromFiles(VkDevice device, const std::map<VkShaderStageFlagBits, std::string>& stagePaths) const
	{
		std::map<VkShaderStageFlagBits, std::vector<char>> byteCodes;
		for (const auto& stagePath : stagePaths)
		{
			const auto spirv = LoadByteCodeFromFile(stagePath.second);
			byteCodes.insert(std::make_pair(stagePath.first, spirv));
		}

		return CreateShaderFromByteCodes(device, byteCodes);
	}

	std::shared_ptr<Shader> ShaderFactory::CreateShaderFromByteCodes(VkDevice device, const std::map<VkShaderStageFlagBits, std::vector<char>>& byteCodes) const
	{
		std::shared_ptr<Shader> shaders = std::make_shared<Shader>(device);

		for (const auto& byteCode : byteCodes)
		{
			shaders->AddShaderModule(byteCode.first, byteCode.second);
		}

		shaders->Init();

		return shaders;
	}
}