#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "Window.h"

class RenderPass;

class SubPass
{
private:
	RenderPass& m_renderPass;

	std::vector<VkAttachmentReference> m_inputReferences;

	std::vector<VkAttachmentReference> m_colorReferences;

	VkAttachmentReference m_depthReference = { UINT32_MAX, VK_IMAGE_LAYOUT_UNDEFINED };

public:
	SubPass(RenderPass& renderpass);

	virtual ~SubPass();

public:
	void UseAttachment(uint32_t attachmentIndex);  // for write

	void UseAttachments(const std::vector<uint32_t>& attachmentIndexes = {});

	void InputAttachment(uint32_t attachmentIndex);  // for read

	void InputAttachments(const std::vector<uint32_t>& attachmentIndices = {});

public:
	operator VkSubpassDescription();
};


class RenderPass
{
private:
	VkDevice m_device;

	VkRenderPass m_renderPass = VK_NULL_HANDLE;

	VkFormat m_surfaceFormat = VK_FORMAT_UNDEFINED;

	VkFormat m_depthFormat = VK_FORMAT_UNDEFINED;

	std::vector<VkClearValue> m_clearValues;

	std::vector<SubPass> m_subpasses;

	std::vector<VkAttachmentDescription> m_attachments;

	std::vector<VkSubpassDependency> m_dependencies;

public:
	RenderPass(VkDevice device);

	~RenderPass();

private:
	static VkAttachmentDescription CreateAttachmentDescription(VkFormat format, VkImageLayout finalLayout);

public:
	uint32_t AddColorAttachment(VkFormat format, VkClearColorValue clearVal = {}, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	uint32_t AddDepthAttachment(VkFormat format, VkClearDepthStencilValue clearVal = { 1.0f, 0 });

	SubPass& AddSubpass(const std::vector<uint32_t>& attachmentIndexes = {});

	void AddSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass);

	void Create();

	void Destroy();

public:
	VkFormat GetSurfaceFormat() const;

	VkFormat GetDepthFormat() const;

	const std::vector<VkClearValue>& GetClearValues() const;

	const std::vector<SubPass>& GetSubPasses() const;

	const std::vector<VkAttachmentDescription>& GetAttachments() const;

	const std::vector<VkSubpassDependency>& GetSubPassDependencies() const;

	operator VkRenderPass();
};

#endif
