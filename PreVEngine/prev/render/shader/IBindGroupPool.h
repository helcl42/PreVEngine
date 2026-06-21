#ifndef __I_BIND_GROUP_POOL_H__
#define __I_BIND_GROUP_POOL_H__

#include "../../core/Core.h"

namespace prev::render::shader {
// Owns the lifecycle of a set of bind groups and decides which slot the next draw (re)creates.
// The caller builds the descriptor (it owns the layout + current bindings) and hands it to
// UpdateNext(); the pool only manages slots and the GfxBindGroup handles in them. Two strategies
// implement this: a fixed-size ring (BindGroupPool), suited to a fixed/one-shot set of bind groups,
// and a per-frame grow-on-demand pool (FrameScopedBindGroupPool) for per-frame rendering. The frame
// signals are required by the interface so a caller can drive any strategy the same way; a ring
// makes them no-ops.
class IBindGroupPool {
public:
    virtual ~IBindGroupPool() = default;

    // Frame-scoped strategies reset their current region here; ring strategies ignore it.
    virtual void BeginFrame(uint32_t frameInFlightIndex) = 0;

    // (Re)create a bind group in the next slot from the given descriptor and return it. The
    // returned handle is valid until the next UpdateNext()/BeginFrame()/EndFrame() reuses its slot.
    virtual GfxBindGroup UpdateNext(const GfxBindGroupDescriptor& descriptor) = 0;

    // Frame-scoped strategies trim this frame's region to what it used; ring strategies ignore it.
    virtual void EndFrame() = 0;
};
} // namespace prev::render::shader

#endif // !__I_BIND_GROUP_POOL_H__
