#include "Image.h"

#include <cstring>

namespace PreVEngine {
Image::Image()
    : m_width(0)
    , m_height(0)
    , m_buffer(nullptr)
{
}

Image::Image(const uint32_t width, const uint32_t height)
    : m_buffer(nullptr)
{
    SetSize(width, height);
}

Image::Image(const uint32_t width, const uint32_t height, const uint8_t* bytes)
    : m_buffer(nullptr)
{
    SetSize(width, height);

    std::memcpy(m_buffer, bytes, static_cast<size_t>(width * height * sizeof(PixelRGBA)));
}

Image::Image(const uint32_t width, const uint32_t height, const PixelRGBA* pixels)
    : m_buffer(nullptr)
{
    SetSize(width, height);

    std::memcpy(m_buffer, pixels, static_cast<size_t>(width * height * sizeof(PixelRGBA)));
}

Image::~Image()
{
    CleanUp();
}

Image::Image(const Image& other)
{
    SetSize(other.m_width, other.m_height);
    std::memcpy(m_buffer, other.m_buffer, static_cast<size_t>(other.m_width * other.m_height * sizeof(PixelRGBA)));

    m_width = other.m_width;
    m_height = other.m_height;
}

Image& Image::operator=(const Image& other)
{
    if (this != &other) {
        SetSize(other.m_width, other.m_height);
        std::memcpy(m_buffer, other.m_buffer, static_cast<size_t>(other.m_width * other.m_height * sizeof(PixelRGBA)));

        m_width = other.m_width;
        m_height = other.m_height;
    }
    return *this;
}

Image::Image(Image&& other)
{
    CleanUp();

    m_buffer = other.m_buffer;
    m_width = other.m_width;
    m_height = other.m_height;

    other.m_buffer = nullptr;
}

Image& Image::operator=(Image&& other)
{
    if (this != &other) {
        CleanUp();

        m_buffer = other.m_buffer;
        m_width = other.m_width;
        m_height = other.m_height;

        other.m_buffer = nullptr;
    }
    return *this;
}

void Image::CleanUp()
{
    if (m_buffer != nullptr) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }
}

void Image::SetSize(const uint32_t w, const uint32_t h)
{
    CleanUp();

    m_width = w;
    m_height = h;
    m_buffer = new PixelRGBA[w * h];
}

PixelRGBA* Image::GetBuffer() const
{
    return m_buffer;
}

uint32_t Image::GetWidth() const
{
    return m_width;
}

uint32_t Image::GetHeight() const
{
    return m_height;
}

void Image::Clear()
{
    const size_t size{ m_width * m_height };
    std::memset(m_buffer, 0, size * sizeof(PixelRGBA));
}

void Image::Clear(const PixelRGBA& color)
{
    const size_t size{ m_width * m_height };
    for (uint32_t i = 0; i < size; i++) {
        m_buffer[i] = color;
    }
}
} // namespace PreVEngine
