#include "Engine.h"
#include "AllocatorProvider.h"
#include "DeviceProvider.h"
#include "device/DeviceFactory.h"

#include "../common/Logger.h"
#include "../core/device/PhysicalDevices.h"
#include "../render/pass/RenderPassBuilder.h"
#include "../scene/Scene.h"
#include "../util/Utils.h"
#include "../util/VkUtils.h"
#include "../window/Window.h"

namespace prev::core {
Engine::Engine(const EngineConfig& config)
    : m_config{ config }
{
}

Engine::~Engine()
{
    ShutDown();
}

void Engine::Init()
{
    ResetTiming();
    ResetInstance();
    ResetWindow();
    ResetSurface();
    ResetDevice();
    ResetAllocator();
    ResetRenderPass();
    ResetSwapchain();

    DeviceProvider::Instance().SetDevice(m_device);
    AllocatorProvider::Instance().SetAllocator(m_allocator);
}

void Engine::InitScene(const std::shared_ptr<prev::scene::IScene>& scene)
{
    m_scene = scene;
    m_scene->Init();
}

void Engine::InitRenderer(const std::shared_ptr<prev::render::IRootRenderer>& rootRenderer)
{
    m_rootRenderer = rootRenderer;
    m_rootRenderer->Init();
}

void Engine::MainLoop()
{
    m_clock->Reset();

    while (m_window->ProcessEvents()) // Main event loop, runs until window is closed.
    {
        prev::event::EventChannel::DispatchQueued();

        m_clock->UpdateClock();
        const auto deltaTime{ m_clock->GetDelta() };

        prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, m_window->GetSize().width, m_window->GetSize().height });

        if (m_window->HasFocus()) {
            m_scene->Update(deltaTime);

            prev::render::SwapChainFrameContext swapchainFrameContext;
            if (m_swapchain->BeginFrame(swapchainFrameContext)) {
                const prev::render::RenderContext renderContext{ swapchainFrameContext.frameBuffer, swapchainFrameContext.commandBuffer, swapchainFrameContext.index, { { 0, 0 }, m_swapchain->GetExtent() } };
                m_rootRenderer->Render(renderContext, m_scene);
                m_swapchain->EndFrame();
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (m_fpsCounter->Tick()) {
            LOGI("FPS %f\n", m_fpsCounter->GetAverageFPS());
        }
    }
}

void Engine::ShutDown()
{
    m_rootRenderer->ShutDown();
    m_scene->ShutDown();

    AllocatorProvider::Instance().SetAllocator(nullptr);
    DeviceProvider::Instance().SetDevice(nullptr);
}

std::shared_ptr<prev::scene::IScene> Engine::GetScene() const
{
    return m_scene;
}

std::shared_ptr<prev::core::device::Device> Engine::GetDevice() const
{
    return m_device;
}

std::shared_ptr<prev::render::Swapchain> Engine::GetSwapchain() const
{
    return m_swapchain;
}

std::shared_ptr<prev::render::pass::RenderPass> Engine::GetRenderPass() const
{
    return m_renderPass;
}

std::shared_ptr<prev::render::IRootRenderer> Engine::GetRootRenderer() const
{
    return m_rootRenderer;
}

void Engine::operator()(const prev::window::WindowChangeEvent& windowChangeEvent)
{
    vkDeviceWaitIdle(*m_device);

    m_swapchain = nullptr; // swapchain needs to be destroyed before surface
    ResetSurface();
    ResetSwapchain();
}

void Engine::operator()(const prev::window::WindowResizeEvent& resizeEvent)
{
    m_swapchain->UpdateExtent();
}

void Engine::ResetTiming()
{
    m_clock = std::make_unique<prev::util::Clock<float>>();
    m_fpsCounter = std::make_unique<prev::util::FPSCounter>();
}

void Engine::ResetInstance()
{
    m_instance = std::make_unique<prev::core::instance::Instance>(m_config.validation);
}

void Engine::ResetWindow()
{
    prev::window::WindowCreateInfo windowCreateInfo{};
    windowCreateInfo.title = m_config.appName;
    windowCreateInfo.fullScreen = m_config.fullScreen;
    windowCreateInfo.left = m_config.windowPosition.x;
    windowCreateInfo.top = m_config.windowPosition.y;
    windowCreateInfo.width = m_config.windowSize.x;
    windowCreateInfo.height = m_config.windowSize.y;

    m_window = std::make_unique<prev::window::Window>(*m_instance, windowCreateInfo);
}

void Engine::ResetSurface()
{
    m_surface = m_window->ResetSurface();
}

void Engine::ResetDevice()
{
    auto physicalDevices{ std::make_shared<prev::core::device::PhysicalDevices>(*m_instance) };
    physicalDevices->Print();

    auto presentablePhysicalDevice{ physicalDevices->FindPresentable(m_surface, m_config.gpuIndex) };
    if (!presentablePhysicalDevice) {
        throw std::runtime_error("No suitable GPU found?!");
    }

    prev::core::device::DeviceFactory deviceFactory{};
    auto device{ deviceFactory.Create(presentablePhysicalDevice, m_surface) };
    if (!device) {
        throw std::runtime_error("Could not create logical device");
    }

    m_device = device;
    m_device->Print();
}

void Engine::ResetAllocator()
{
    // TODO
    //  -> Allocator uses only graphics queue for it's internal commands
    //     - make queue type as a parameter or even better decouple it from queue completely
    m_allocator = std::make_shared<prev::core::memory::Allocator>(*m_instance, *m_device, *m_device->GetQueue(device::QueueType::GRAPHICS)); // Create "Vulkan Memory Allocator"
    LOGI("Allocator created\n");
}

void Engine::ResetRenderPass()
{
    prev::render::pass::RenderPassBuilder renderPassBuilder{ *m_device, m_config.viewCount };

    const auto colorFormat{ m_device->GetGPU()->FindSurfaceFormat(m_surface) };
    const auto depthFormat{ m_device->GetGPU()->FindDepthFormat() };
    const VkClearColorValue clearColor{ { 0.5f, 0.5f, 0.5f, 1.0f } };
    const VkSampleCountFlagBits sampleCount{ prev::util::vk::GetSampleCountBit(m_config.samplesCount) };

    if (sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        std::vector<VkSubpassDependency> dependencies(2);
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].dstSubpass = 0;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        m_renderPass = renderPassBuilder
                           .AddColorAttachment(colorFormat, sampleCount, clearColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE) // color buffer, multisampled
                           .AddDepthAttachment(depthFormat, sampleCount, { MAX_DEPTH, 0 }, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE) // depth buffer, multisampled
                           .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, true) // color buffer, resolve buffer
                           .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, { MAX_DEPTH, 0 }, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, true) // depth buffer, resolve buffer
                           .AddSubpass({ 0, 1 }, { 2, 3 }) // resolve ref will be at index 2 & 3
                           .AddSubpassDependencies(dependencies)
                           .Build();
    } else {
        std::vector<VkSubpassDependency> dependencies(2);
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_NONE;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].dstSubpass = 0;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_NONE;
        dependencies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        m_renderPass = renderPassBuilder
                           .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor)
                           .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, { MAX_DEPTH, 0 })
                           .AddSubpass({ 0, 1 })
                           .AddSubpassDependencies(dependencies)
                           .Build();
    }
}

void Engine::ResetSwapchain()
{
    m_swapchain = std::make_shared<prev::render::Swapchain>(*m_device, *m_allocator, *m_renderPass, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount), m_config.viewCount);
#if defined(__ANDROID__)
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR);
#else
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
    m_swapchain->SetImageCount(m_config.swapchainFrameCount);
    m_swapchain->Print();
}
} // namespace prev::core