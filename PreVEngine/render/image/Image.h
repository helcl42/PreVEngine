#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "PixelRGBA.h"

namespace prev {

class Image {
public:
    Image();

    Image(const uint32_t width, const uint32_t height);

    Image(const uint32_t width, const uint32_t height, const uint8_t* bytes);

    Image(const uint32_t width, const uint32_t height, const PixelRGBA* pixels);

    ~Image();

public:
    Image(const Image& other);

    Image& operator=(const Image& other);

    Image(Image&& other);

    Image& operator=(Image&& other);

private:
    void CleanUp();

public:
    void SetSize(const uint32_t width, const uint32_t height);

    uint32_t GetWidth() const;

    uint32_t GetHeight() const;

    PixelRGBA* GetBuffer() const;

    void Clear();

    void Clear(const PixelRGBA& color);

private:
    uint32_t m_width;

    uint32_t m_height;

    PixelRGBA* m_buffer;
};
} // namespace prev

#endif
