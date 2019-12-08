#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <RenderPass.h>
#include <Shader.h>

using namespace PreVEngine;

class IGraphicsPipeline
{
public:
	virtual VkPipeline Init() = 0;

	virtual void ShutDown() = 0;

	virtual VkPipelineLayout GetLayout() const = 0;

	virtual operator VkPipeline() const = 0;

public:
	virtual ~IGraphicsPipeline() {}
};

class AbstractGraphicsPipeline : public IGraphicsPipeline
{
protected:
	VkDevice m_device;

	VkRenderPass m_renderPass;

	Shader& m_shaders;

	VkPipeline m_graphicsPipeline;

	VkPipelineLayout m_pipelineLayout;

protected:
	AbstractGraphicsPipeline(VkDevice device, VkRenderPass renderpass, Shader& shaders);

	virtual ~AbstractGraphicsPipeline();

public:
	virtual void ShutDown() override;

public:
	VkPipelineLayout GetLayout() const;

	operator VkPipeline() const;
};


class ShadowsPipeline final : public AbstractGraphicsPipeline
{
public:
	ShadowsPipeline(VkDevice device, VkRenderPass renderpass, Shader& shaders);

	~ShadowsPipeline();

public:
	VkPipeline Init() override;
};


class DefaultPipeline final : public AbstractGraphicsPipeline
{

public:
	DefaultPipeline(VkDevice device, VkRenderPass renderpass, Shader& shaders);

	~DefaultPipeline();

public:
	VkPipeline Init() override;
};


class QuadPipeline final : public AbstractGraphicsPipeline
{

public:
	QuadPipeline(VkDevice device, VkRenderPass renderpass, Shader& shaders);

	~QuadPipeline();

public:
	VkPipeline Init() override;
};

#endif

