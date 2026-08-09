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
    m_mapOutstanding.assign(m_poolCount, false);
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

    // Unmap every slot with an outstanding map before destroying
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        if (m_mapOutstanding[i] && m_resultBuffers[i]) {
            UnmapSlot(i);
        }
    }
    m_asyncMapSlot = -1;

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
    m_queryRecorded = true;
}

void QueryPool::EndQuery(const uint32_t queryIndex, GfxRenderPassEncoder renderPassEncoder)
{
    gfxRenderPassEncoderEndOcclusionQuery(renderPassEncoder);
}

void QueryPool::WriteTimestamp(GfxCommandEncoder commandEncoder, uint32_t queryIndex)
{
    gfxCommandEncoderWriteTimestamp(commandEncoder, m_querySets[m_index], queryIndex);
    m_queryRecorded = true;
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
    m_index.Reset();
    m_asyncMapSlot = -1; // indices rewound: the async read restarts (outstanding maps stay marked)
}

void QueryPool::Resolve(GfxCommandEncoder commandEncoder)
{
    if (!m_queryRecorded) {
        // Nothing recorded into this set (e.g. the occluder was not drawn): resolving would copy an
        // unavailable query. The set stays current - the caller resets it before each pass.
        return;
    }
    m_queryRecorded = false;

    ReclaimOutstanding();
    if (m_mapOutstanding[m_index]) {
        ++m_index; // still mapped: a copy into it would be invalid, so skip the slot (sample dropped)
        return;
    }

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
    ++m_index; // advance so the CPU can read this result while the next frame writes the next set
}

bool QueryPool::MapSlot(const uint32_t slot, const uint64_t offset, const uint64_t size, void*& outPointer)
{
    if (!m_resultBuffers[slot]) {
        return false;
    }
    const GfxResult result{ gfxBufferMapAsync(*m_resultBuffers[slot], offset, size, &outPointer) };
    m_mapOutstanding[slot] = (result == GFX_RESULT_SUCCESS || result == GFX_RESULT_NOT_READY);
    return result == GFX_RESULT_SUCCESS && outPointer;
}

void QueryPool::UnmapSlot(const uint32_t slot)
{
    gfxBufferUnmap(*m_resultBuffers[slot]);
    m_mapOutstanding[slot] = false;
}

void QueryPool::ReclaimOutstanding()
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        if (!m_mapOutstanding[i] || static_cast<int32_t>(i) == m_asyncMapSlot) {
            continue; // the tracked async read is the consumer's to collect
        }
        void* pointer{ nullptr };
        if (MapSlot(i, 0, sizeof(uint64_t) * m_queryCount, pointer)) {
            UnmapSlot(i);
        }
    }
}

QueryPool::operator GfxQuerySet() const
{
    return m_querySets[m_index];
}
} // namespace prev::render::query
