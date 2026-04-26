#include "QueryPool.h"

#include "../../common/Logger.h"

namespace prev::render::query {
QueryPool::QueryPool(prev::core::device::Device& device, GfxQueryType queryType, uint32_t poolCount, uint32_t queryCount)
    : m_device{ device }
    , m_queryType{ queryType }
    , m_poolCount{ poolCount }
    , m_queryCount{ queryCount }
    , m_index{ prev::util::CircularIndex<uint32_t>(poolCount) }
{
    m_querySets.resize(m_poolCount);
    m_resultBuffers.resize(m_poolCount);
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        GfxQuerySetDescriptor desc{};
        desc.sType = GFX_STRUCTURE_TYPE_QUERY_SET_DESCRIPTOR;
        desc.type = m_queryType;
        desc.count = m_queryCount;
        GFXERRCHECK(gfxDeviceCreateQuerySet(m_device, &desc, &m_querySets[i]));

        // Create host-mapped result buffer for each pool
        GfxBufferDescriptor bufDesc{};
        bufDesc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
        bufDesc.usage = GFX_BUFFER_USAGE_COPY_DST | GFX_BUFFER_USAGE_MAP_READ;
        bufDesc.size = sizeof(uint64_t) * m_queryCount;
        bufDesc.memoryProperties = GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT;
        GFXERRCHECK(gfxDeviceCreateBuffer(m_device, &bufDesc, &m_resultBuffers[i]));
    }
}

QueryPool::~QueryPool()
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        if (m_resultBuffers[i]) {
            gfxBufferDestroy(m_resultBuffers[i]);
        }
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

void QueryPool::Reset(GfxCommandEncoder commandEncoder)
{
    gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount);
}

void QueryPool::ResetAll(GfxCommandEncoder commandEncoder)
{
    for (uint32_t i = 0; i < m_poolCount; ++i) {
        gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[i], 0, m_queryCount);
    }
    m_index.Reset();
}

void QueryPool::Resolve(GfxCommandEncoder commandEncoder)
{
    gfxCommandEncoderResolveQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount, m_resultBuffers[m_index], 0);
    ++m_index;
    // Reset the next query set so it's ready for use
    gfxCommandEncoderResetQuerySet(commandEncoder, m_querySets[m_index], 0, m_queryCount);
}

QueryPool::operator GfxQuerySet() const
{
    return m_querySets[m_index];
}
} // namespace prev::render::query
