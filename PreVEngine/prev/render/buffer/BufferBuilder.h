#ifndef __BUFFER_BUILDER_H__
#define __BUFFER_BUILDER_H__

#include "Buffer.h"

#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <memory>

namespace prev::render::buffer {
class BufferBuilder final {
public:
    BufferBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue);

    ~BufferBuilder() = default;

public:
    BufferBuilder& SetUsageFlags(const GfxBufferUsageFlags usageFlags);

    BufferBuilder& SetHostMapped(bool hostMapped);

    BufferBuilder& SetSize(const uint64_t size);

    BufferBuilder& SetAlignment(const uint64_t alignment);

    BufferBuilder& SetData(const void* data, const uint64_t size);

    std::unique_ptr<Buffer> Build() const;

private:
    void Validate() const;

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxBufferUsageFlags m_usageFlags{};

    bool m_hostMapped{ false };

    uint64_t m_size{ 0 };

    uint64_t m_alignment{ 8 };

    const void* m_data{ nullptr };

    uint64_t m_dataSize{};
};
} // namespace prev::render::buffer

#endif // !__BUFFER_BUILDER_H__