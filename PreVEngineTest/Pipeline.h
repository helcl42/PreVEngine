#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <RenderPass.h>
#include <Shader.h>

class Pipeline
{
	VkDevice     device;
	VkRenderPass renderpass;
	VkPipeline   graphicsPipeline;
	PreVEngine::Shader*    shaders;

public:
	VkPipelineLayout pipelineLayout;

	Pipeline(VkDevice device, VkRenderPass renderpass, PreVEngine::Shader& shaders);
	~Pipeline();

	VkPipeline CreateGraphicsPipeline();

	operator VkPipeline() const
	{
		return graphicsPipeline;
	}
};

#endif

