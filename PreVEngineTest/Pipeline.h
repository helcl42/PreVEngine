#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "Window.h"
#include "Swapchain.h"
#include "Shaders.h"

class Pipeline
{
	VkDevice     device;
	VkRenderPass renderpass;
	VkPipeline   graphicsPipeline;
	Shaders*    shaders;

public:
	VkPipelineLayout pipelineLayout;

	Pipeline(VkDevice device, VkRenderPass renderpass, Shaders& shaders);
	~Pipeline();

	VkPipeline CreateGraphicsPipeline();

	operator VkPipeline() const
	{
		return graphicsPipeline;
	}
};

#endif

