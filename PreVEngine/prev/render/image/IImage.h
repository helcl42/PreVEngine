#ifndef __IIMAGE_H__
#define __IIMAGE_H__

#include <cinttypes>

namespace prev::render::image {

class IImage {
public:
    virtual ~IImage() = default;

public:
    virtual uint32_t GetWidth() const = 0;

    virtual uint32_t GetHeight() const = 0;

    virtual uint32_t GetChannels() const = 0;

    virtual uint32_t GetSize() const = 0;

    virtual uint32_t GetPixelSize() const = 0;

    virtual uint32_t GetBitDepth() const = 0;

    virtual bool IsFloatingPoint() const = 0;

    virtual uint8_t* GetRawDataPtr() = 0;

    virtual const uint8_t* GetRawDataPtr() const = 0;
};

} // namespace prev::render::image

#endif
