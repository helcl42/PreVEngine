#include "VertexBuffer.h"

namespace prev {
void VBO::Data(const void* data, const uint32_t count, const uint32_t stride)
{
    Buffer::Data(data, count, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}
} // namespace prev