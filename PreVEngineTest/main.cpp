#include <iostream>

#include <Window.h>
#include <Devices.h>
#include <RenderPass.h>
#include <Swapchain.h>
#include <Pipeline.h>
#include <Buffers.h>
#include <Image.h>

#include "matrix.h"
#include "Shaders.h"

float dx = 0.1f;
float dy = 0.2f;
float translateX = 0.0f;

int windowWidth = 1280;
int windowHeight = 960;
bool windowResized = false;

//-- EVENT HANDLERS --
class CWindow : public Window
{
	float mx = 0;
	float my = 0;

public:
	CWindow(const char* title)
		: Window(title)
	{
	}

	CWindow(const char* title, const uint32_t width, const uint32_t height)
		: Window(title, width, height)
	{
	}

public:
	void OnResizeEvent(uint16_t width, uint16_t height)
	{
		printf("OnResizeEvent: %d x %d\n", width, height);
		windowWidth = width;
		windowHeight = height;
		windowResized = true;
	}

	void OnKeyEvent(eAction action, eKeycode keycode)
	{
		if (action == eDOWN)
		{
			if (keycode == KEY_Left)
			{
				dy -= 0.1f;
			}

			if (keycode == KEY_Right)
			{
				dy += 0.1f;
			}

			if (keycode == KEY_Up)
			{
				dx += 0.1f;
			}

			if (keycode == KEY_Down)
			{
				dx -= 0.1f;
			}
		}
	}

	void OnMouseEvent(eAction action, int16_t x, int16_t y, uint8_t btn)
	{
		if (action == eMOVE && btn == 1)
		{
			dy = x - mx;
			dx = my - y;
		}

		mx = x;
		my = y;
	}

	void OnTouchEvent(eAction action, float x, float y, uint8_t id)
	{
		if (action == eMOVE)
		{
			dy = x - mx;
			dx = my - y;
		}

		mx = x;
		my = y;
	}

	void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y)
	{
		std::cout << "Mouse scroll: " << delta << std::endl;
	}

	void OnFocusEvent(bool hasFocus)
	{
		std::cout << "Window Focus changed to: " << (hasFocus ? "YES" : "NO") << std::endl;
	}

	void OnInitEvent()
	{
		std::cout << "Window initialized" << std::endl;
	}

	void OnCloseEvent()
	{
		std::cout << "Window closed" << std::endl;
	}
};


struct Vertex
{
	vec3 pos;
	vec2 tc;
};

struct Mesh
{
	const std::vector<Vertex> vertices = {
		//front
		{{-0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
		//back
		{{ 0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{-0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{-0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
		//left
		{{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{-0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
		//right
		{{ 0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
		//top
		{{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f,-0.5f, 0.5f}, {0.0f, 1.0f}},
		//bottom
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		8, 9,10, 10,11, 8,
	   12,13,14, 14,15,12,
	   16,17,18, 18,19,16,
	   20,21,22, 22,23,20
	};
};

struct TexturedModel
{
	std::shared_ptr<Mesh> mesh;

	std::shared_ptr<Image> texture;

	std::shared_ptr<ImageBuffer> imageBuffer;

	std::shared_ptr<VBO> vertexBuffer;

	std::shared_ptr<IBO> indexBuffer;

	mat4 transform;
};

class ModelFactory
{
public:
	std::shared_ptr<TexturedModel> CreateTexturedModel(Allocator& allocator, const std::string& textureFilename, const mat4& transform)
	{
		ImageFactory imageFactory;

		auto resultModel = std::make_shared<TexturedModel>();

		resultModel->texture = imageFactory.CreateImage(textureFilename);

		VkExtent3D extent1 = { resultModel->texture->GetWidth(), resultModel->texture->GetHeight(), 1 };

		resultModel->imageBuffer = std::make_shared<ImageBuffer>(allocator);
		resultModel->imageBuffer->Data(resultModel->texture->GetBuffer(), extent1, VK_FORMAT_R8G8B8A8_UNORM, true);

		resultModel->mesh = std::make_shared<Mesh>();

		resultModel->vertexBuffer = std::make_shared<VBO>(allocator);                                                          // Create vertex buffer
		resultModel->vertexBuffer->Data((void*)resultModel->mesh->vertices.data(), (uint32_t)resultModel->mesh->vertices.size(), sizeof(Vertex));  // load vertex data
		printf("VBO created\n");

		resultModel->indexBuffer = std::make_shared<IBO>(allocator);
		resultModel->indexBuffer->Data(resultModel->mesh->indices.data(), (uint32_t)resultModel->mesh->indices.size());
		printf("IBO created\n");

		resultModel->transform = transform;

		return resultModel;
	}
};

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);										// avoid buffering
	
	Instance instance(true);

	CWindow Window("PreVEngine", 1280, 960);
	Window.SetTitle("PreVEngine Demo");
	Window.SetWinSize(windowWidth, windowHeight);
	Window.SetWinPos(40, 40);

	PhysicalDevices gpus(instance);
	gpus.Print();

	VkSurfaceKHR surface = Window.GetSurface(instance);
	PhysicalDevice *gpu = gpus.FindPresentable(surface);					// get presenting GPU?
	if (!gpu)
	{
		throw std::runtime_error("No GPU found?!");
	}

	Device device(*gpu);

	Queue* presenQueue = device.AddQueue(VK_QUEUE_GRAPHICS_BIT, surface);   // graphics + present-queue
	Queue* graphicsQueue = presenQueue;                                     // they might be the same or not
	if (!presenQueue)
	{
		presenQueue = device.AddQueue(0, surface);                          // create present-queue
		graphicsQueue = device.AddQueue(VK_QUEUE_GRAPHICS_BIT);             // create graphics queue
	}

	device.Print();
	
	const uint32_t BUFFERS_IN_FLIGHT = 3;									// use tripple-buffering

	VkFormat colorFormat = gpu->FindSurfaceFormat(surface);
	VkFormat depthFormat = gpu->FindDepthFormat();

	RenderPass renderpass(device);
	renderpass.AddColorAttachment(colorFormat, { 0.0f, 0.0f, 0.3f, 1.0f });	// color buffer, clear to blue
	renderpass.AddDepthAttachment(depthFormat);
	renderpass.AddSubpass({ 0, 1 });
	
	Swapchain swapchain(renderpass, presenQueue, graphicsQueue);
	swapchain.SetImageCount(BUFFERS_IN_FLIGHT);
	swapchain.Print();

	////////////////////////////////////////////////////////
	// Client Init Code: starts here
	////////////////////////////////////////////////////////

	Allocator allocator(*graphicsQueue);                                        // Create "Vulkan Memory Aloocator"
	printf("Allocator created\n");

	const uint32_t COUNT_OF_MODELS = 10;
	std::vector<std::shared_ptr<TexturedModel>> models(COUNT_OF_MODELS);

	mat4 trasform;
	trasform.Translate(-(0.1f * COUNT_OF_MODELS), 0.0f, 0.0f);

	ModelFactory modelFactory;
	for (uint32_t i = 0; i < COUNT_OF_MODELS; i++)
	{
		trasform.Translate(0.2f, 0.0f, 0.0f);
		models[i] = modelFactory.CreateTexturedModel(allocator, i % 2 == 0 ? "vulkan.png" : "texture.jpg", trasform);
	}

	// Uniform Buffer Object
	struct Uniforms
	{
		mat4 model;
		mat4 view;
		mat4 proj;
	};

	Uniforms uniforms;
	uniforms.view.Translate(0, 0, -4);

	std::vector<std::shared_ptr<UBO>> uniformBuffers(models.size());
	for (size_t i = 0; i < models.size(); i++)
	{
		uniformBuffers[i] = std::make_shared<UBO>(allocator);
		uniformBuffers[i]->Allocate(sizeof(Uniforms));
		printf("UBO %zd created\n", i);
	}

	Shaders shaders(device);
	shaders.AddShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv");
	shaders.AddShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv");
	shaders.Init();
	shaders.AdjustDescriptorsSetsCapacity(COUNT_OF_MODELS * BUFFERS_IN_FLIGHT);

	Pipeline pipeline(device, renderpass, shaders);
	pipeline.CreateGraphicsPipeline();
	printf("Pipeline created\n");

	int descriptorSetIndex = 0;	

	////////////////////////////////////////////////////////
	// Client Init Code: ends here
	////////////////////////////////////////////////////////

	while (Window.ProcessEvents()) // Main event loop, runs until window is closed.
	{
		if (windowResized)
		{
			swapchain.UpdateExtent();
			windowResized = false;
		}

		VkExtent2D ext = swapchain.GetExtent();
		VkRect2D   scissor = { {0, 0}, ext };
		VkViewport viewport = { 0, 0, (float)ext.width, (float)ext.height, 0, 1 };

		float aspect = (float)ext.width / (float)ext.height;

		// Call Update

		VkCommandBuffer commandBuffer;
		if (swapchain.BeginFrame(commandBuffer))
		{			
			// Call Render

			// Common
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			size_t modelIndex = 0;
			for (const auto& model : models)
			{
				VkBuffer vertexBuffers[] = { *model->vertexBuffer };
				VkDeviceSize offsets[] = { 0 };

				auto& ubo = uniformBuffers.at(modelIndex);

				uniforms.proj.SetProjection(aspect, 40.f, 1, 1000);
				uniforms.model = model->transform;
				uniforms.model.RotateX(dx);
				uniforms.model.RotateY(dy);
				model->transform = uniforms.model;
				ubo->Update(&uniforms);

				// descriptor set for Cube 1
				shaders.Bind("texSampler", *model->imageBuffer);
				shaders.Bind("ubo", *ubo);
				VkDescriptorSet descriptorSet = shaders.UpdateDescriptorSet(descriptorSetIndex);
				descriptorSetIndex++;

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, *model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, model->indexBuffer->GetCount(), 1, 0, 0, 0);

				modelIndex++;
			}

			swapchain.EndFrame();

			descriptorSetIndex %= COUNT_OF_MODELS * BUFFERS_IN_FLIGHT;
		}
	}

	//-----------------
	return 0;
}
