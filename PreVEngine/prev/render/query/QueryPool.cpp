#include "QueryPool.h"

#include "../../common/Logger.h"
#include "../buffer/BufferBuilder.h"

namespace prev::render::query {
QueryPool::QueryPool(prev::core::device::Device& device, GfxQueryType queryType, uint32_t poolCount, uint32_t queryCount, bool precise)
    : m_device{ device }
    , m_queryType{ queryType }
    , m_poolCount{ poolCount }
    , m_queryCount{ queryCount }
    , m_index{ prev::util::CircularIndex<uint32_t>(poolCount) }
{
    m_querySets.resize(m_poolCount);
    m_resolveBuffers.resize(m_poolCount);
    m_resultBuffers.resize(m_poolCount);
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        GfxOcclusionQueryDescriptor occlusionDesc{};
        occlusionDesc.sType = GFX_STRUCTURE_TYPE_OCCLUSION_QUERY_DESCRIPTOR;
        occlusionDesc.pNext = nullptr;
        occlusionDesc.mode = precise ? GFX_OCCLUSION_QUERY_MODE_PRECISE : GFX_OCCLUSION_QUERY_MODE_BOOLEAN;

        GfxQuerySetDescriptor desc{};
        desc.sType = GFX_STRUCTURE_TYPE_QUERY_SET_DESCRIPTOR;
        desc.pNext = (m_queryType == GFX_QUERY_TYPE_OCCLUSION) ? &occlusionDesc : nullptr;
        desc.type = m_queryType;
        desc.count = m_queryCount;
        GFXERRCHECK(gfxDeviceCreateQuerySet(m_device, &desc, &m_querySets[i]));

        // Buffer for resolving query results (GPU-side)
        m_resolveBuffers[i] = prev::render::buffer::BufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                  .SetSize(sizeof(uint64_t) * m_queryCount)
                                  .SetUsageFlags(GFX_BUFFER_USAGE_QUERY_RESOLVE | GFX_BUFFER_USAGE_COPY_SRC | GFX_BUFFER_USAGE_COPY_DST)
                                  .SetMemoryProperties(GFX_MEMORY_PROPERTY_DEVICE_LOCAL)
                                  .Build();

        // Staging buffer for CPU readback
        m_resultBuffers[i] = prev::render::buffer::BufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                 .SetSize(sizeof(uint64_t) * m_queryCount)
                                 .SetUsageFlags(GFX_BUFFER_USAGE_COPY_DST | GFX_BUFFER_USAGE_MAP_READ)
                                 .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                                 .Build();
    }
}

QueryPool::~QueryPool()
{
    m_device.WaitIdle();

    // Unmap any pending async-mapped buffer before destroying
    if (m_asyncMapPending && m_resultBuffers[m_asyncMapIndex]) {
        gfxBufferUnmap(*m_resultBuffers[m_asyncMapIndex]);
        m_asyncMapPending = false;
    }

    for (uint32_t i = 0; i < m_poolCount; ++i) {
        m_resultBuffers[i].reset();
        m_resolveBuffers[i].reset();
        if (m_querySets[i]) {
            gfxQuerySetDestroy(m_querySets[i]);
        }
    }
}

void QueryPool::BeginQuery(const uint32_t queryIndex, GfxRenderPassEncoder renderPassEncoder)
{
    gfxRenderPassEncoderBeginOcclusionQuery(renderPassEncoder, m_querySets[m_index], queryIndex);
}

void QueryPool::EndQuery(const uint32_t queryIndex, GfxRenderPassEncoder renderPassEncoder)
{
    gfxRenderPassEncoderEndOcclusionQuery(renderPassEncoder);
}

void QueryPool::WriteTimestamp(GfxCommandEncoder commandEncoder, uint32_t queryIndex)
{
    gfxCommandEncoderWriteTimestamp(commandEncoder, m_querySets[m_index], queryIndex);
}

void QueryPool::Reset(GfxCommandEncoder commandEncoder)
{
    gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount);
}

void QueryPool::ResetAll(GfxCommandEncoder commandEncoder)
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[i], 0, m_queryCount);
    }
    m_readIndex = 0;
    m_hasResolved = false;
    m_index.Reset();
}

void QueryPool::Resolve(GfxCommandEncoder commandEncoder)
{
    gfxCommandEncoderResolveQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount, *m_resolveBuffers[m_index], 0);
    // Copy from resolve buffer to mappable staging buffer
    GfxCopyBufferToBufferDescriptor copyDesc{};
    copyDesc.source = *m_resolveBuffers[m_index];
    copyDesc.sourceOffset = 0;
    copyDesc.destination = *m_resultBuffers[m_index];
    copyDesc.destinationOffset = 0;
    copyDesc.size = sizeof(uint64_t) * m_queryCount;
    gfxCommandEncoderCopyBufferToBuffer(commandEncoder, &copyDesc);
    m_readIndex = m_index; // remember which slot holds the latest result
    m_hasResolved = true;
    ++m_index;
    // Reset the next query set so it's ready for use
    gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount);
}

QueryPool::operator GfxQuerySet() const
{
    return m_querySets[m_index];
}
} // namespace prev::render::query
