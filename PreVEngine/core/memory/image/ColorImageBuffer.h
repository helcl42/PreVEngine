#ifndef __COLOR_IMAGE_BUFFER_H__
#define __COLOR_IMAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace PreVEngine {
class ColorImageBuffer : public AbstractImageBuffer {
public:
    ColorImageBuffer(Allocator& allocator);

    ~ColorImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace PreVEngine

#endif // !__COLOR_IMAGE_BUFFER_H__
