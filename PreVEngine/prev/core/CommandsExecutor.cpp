#include "CommandsExecutor.h"

#include "OwnedGfxHandle.h"
#include "sync/Fence.h"

#include "../common/Common.h"
#include "../common/Logger.h"

namespace prev::core {
CommandsExecutor::CommandsExecutor(const device::Device& device, const device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
{
}

GfxCommandEncoder CommandsExecutor::RecordCommands(const std::function<void(GfxCommandEncoder)>& func) const
{
    GfxCommandEncoderDescriptor ceDesc{};
    ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
    ceDesc.label = "OneShotCommandEncoder";
    GfxCommandEncoder commandEncoder{};
    GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &commandEncoder));

    GFXERRCHECK(gfxCommandEncoderBegin(commandEncoder));
    func(commandEncoder);
    GFXERRCHECK(gfxCommandEncoderEnd(commandEncoder));
    return commandEncoder;
}

void CommandsExecutor::ExecuteImmediate(const std::function<void(GfxCommandEncoder)>& func)
{
    const GfxCommandEncoder commandEncoder{ RecordCommands(func) };
    GfxCommandEncoder encoders[] = { commandEncoder };

    const bool useFence{ SUPPORTS_BLOCKING_GPU_WAIT }; // without it, queue ordering covers the callers

    sync::Fence fence{ m_device, false, "ImmediateFence" };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.signalFence = useFence ? static_cast<GfxFence>(fence) : GfxFence{};
    GFXERRCHECK(m_queue.Submit(&submitDesc));

    if (useFence) {
        fence.Wait(); // the wait is what makes destroying the encoder on return legal on Vulkan
    }
    gfxCommandEncoderDestroy(commandEncoder);
}

void CommandsExecutor::ExecuteDeferred(DeferredResourceDestroyer& destroyer, const std::function<void(GfxCommandEncoder)>& func)
{
    const GfxCommandEncoder commandEncoder{ RecordCommands(func) };
    GfxCommandEncoder encoders[] = { commandEncoder };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    GFXERRCHECK(m_queue.Submit(&submitDesc));

    destroyer.Destroy(std::make_unique<OwnedGfxCommandEncoder>(commandEncoder));
}
} // namespace prev::core
