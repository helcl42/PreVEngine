#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cstring>
#include <iostream>
#include <memory>
#include <stdint.h>

namespace PreVEngine {
struct RGBA {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;

    RGBA();

    RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    RGBA Lerp(const RGBA& c, const float f);
};

class Image {
private:
    uint32_t m_width;

    uint32_t m_height;

    std::shared_ptr<RGBA[]> m_buffer;

public:
    Image();

    Image(const int width, const int height);

    Image(const int width, const int height, const uint8_t* bytes);

    Image(const int width, const int height, const RGBA* pixels);

    ~Image();

public:
    Image(const Image& other);

    Image& operator=(const Image& other);

    Image(Image&& other);

    Image& operator=(Image&& other);

private:
    void CleanUp();

public:
    void SetSize(const int width, const int height);

    uint32_t GetWidth() const;

    uint32_t GetHeight() const;

    std::shared_ptr<RGBA[]> GetBuffer() const;

    void Clear();

    void Clear(const RGBA& color);
};

class ImageFactory {
private:
    static bool FileExists(const std::string& fileName);

public:
    std::unique_ptr<Image> CreateImage(const std::string& filename, bool flipVertically = false) const;

    std::unique_ptr<Image> CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient = true, const int checkers = 64) const;

    std::unique_ptr<Image> CreateImageWithColor(const uint32_t width, const uint32_t height, const RGBA& color) const;
};
} // namespace PreVEngine

#endif
