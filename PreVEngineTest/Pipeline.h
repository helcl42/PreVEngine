#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <RenderPass.h>
#include <Shader.h>

class Pipeline
{
private:
	VkDevice m_device;

	VkRenderPass m_renderPass;

	PreVEngine::Shader& m_shaders;

	VkPipeline m_graphicsPipeline;

	VkPipelineLayout m_pipelineLayout;

public:
	Pipeline(VkDevice device, VkRenderPass renderpass, PreVEngine::Shader& shaders);

	~Pipeline();

public:
	VkPipeline CreateGraphicsPipeline();

public:
	VkPipelineLayout GetPipelineLayout() const;

	operator VkPipeline() const;
};

#endif

