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

    GfxFenceDescriptor fenceDesc{};
    fenceDesc.sType = GFX_STRUCTURE_TYPE_FENCE_DESCRIPTOR;
    fenceDesc.label = "ImmediateFence";
    fenceDesc.signaled = false;
    GFXERRCHECK(gfxDeviceCreateFence(m_device, &fenceDesc, &m_fence));
}

CommandsExecutor::~CommandsExecutor()
{
    if (m_fence) {
        gfxFenceDestroy(m_fence);
    }
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
    submitDesc.signalFence = m_fence;
    GFXERRCHECK(m_queue.Submit(&submitDesc));

    GFXERRCHECK(gfxFenceWait(m_fence, UINT64_MAX));
    GFXERRCHECK(gfxFenceReset(m_fence));
}
} // namespace prev::core
