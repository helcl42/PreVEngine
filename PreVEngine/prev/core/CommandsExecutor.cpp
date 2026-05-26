#include "CommandsExecutor.h"

#include "../common/Logger.h"

namespace prev::core {
CommandsExecutor::CommandsExecutor(const device::Device& device, const device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
{
    GfxCommandEncoderDescriptor ceDesc{};
    ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
    ceDesc.label = "ImmediateCommandEncoder";
    GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &m_commandEncoder));

    m_fence = std::make_unique<sync::Fence>(m_device, false, "ImmediateFence");
}

CommandsExecutor::~CommandsExecutor()
{
    m_fence.reset();
    if (m_commandEncoder) {
        gfxCommandEncoderDestroy(m_commandEncoder);
    }
}

void CommandsExecutor::ExecuteImmediate(const std::function<void(GfxCommandEncoder)>& func)
{
    GFXERRCHECK(gfxCommandEncoderBegin(m_commandEncoder));

    func(m_commandEncoder);

    GFXERRCHECK(gfxCommandEncoderEnd(m_commandEncoder));

    GfxCommandEncoder encoders[] = { m_commandEncoder };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.signalFence = *m_fence;
    GFXERRCHECK(m_queue.Submit(&submitDesc));

    m_fence->Wait();
    m_fence->Reset();
}
} // namespace prev::core
