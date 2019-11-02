#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "Window.h"
#include "Swapchain.h"
#include "Shader.h"

class Pipeline
{
	VkDevice     device;
	VkRenderPass renderpass;
	VkPipeline   graphicsPipeline;
	Shader*    shaders;

public:
	VkPipelineLayout pipelineLayout;

	Pipeline(VkDevice device, VkRenderPass renderpass, Shader& shaders);
	~Pipeline();

	VkPipeline CreateGraphicsPipeline();

	operator VkPipeline() const
	{
		return graphicsPipeline;
	}
};

#endif

