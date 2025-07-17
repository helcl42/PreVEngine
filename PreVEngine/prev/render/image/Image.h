#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "IImage.h"
#include "ImageFormat.h"
#include "Pixel.h"

#include <stdexcept>
#include <vector>

namespace prev::render::image {

template <typename T, size_t Channels>
class Image : public IImage {
public:
    using PixelType = Pixel<T, Channels>;

    Image()
        : m_width(0)
        , m_height(0)
        , m_pixels()
    {
    }

    Image(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height)
        , m_pixels(width * height)
    {
        if (width == 0 || height == 0) {
            throw std::invalid_argument("Image dimensions must be positive.");
        }
    }

    Image(uint32_t width, uint32_t height, const T* data)
        : m_width(width)
        , m_height(height)
        , m_pixels(width * height)
    {
        if (width == 0 || height == 0) {
            throw std::invalid_argument("Image dimensions must be positive.");
        }

        if (data == nullptr) {
            throw std::invalid_argument("Image data cannot be null.");
        }

        for (size_t i = 0; i < m_pixels.size(); ++i) {
            m_pixels[i] = PixelType(data + i * Channels);
        }
    }

    ~Image() = default;

    Image(const Image&) = default;
    Image& operator=(const Image&) = default;

    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;

public:
    uint32_t GetWidth() const override
    {
        return m_width;
    }

    uint32_t GetHeight() const override
    {
        return m_height;
    }

    uint32_t GetChannels() const override
    {
        return Channels;
    }

    uint32_t GetSize() const override
    {
        return m_pixels.size();
    }

    uint32_t GetPixelSize() const override
    {
        return static_cast<uint32_t>(sizeof(PixelType));
    }

    uint32_t GetBitDepth() const override
    {
        return GetPixelSize() * 8;
    }

    bool IsFloatingPoint() const override
    {
        return std::is_floating_point_v<T>;
    }

    uint8_t* GetRawDataPtr() override
    {
        return reinterpret_cast<uint8_t*>(GetRawData());
    }

    const uint8_t* GetRawDataPtr() const override
    {
        return reinterpret_cast<const uint8_t*>(GetRawData());
    }

    T* GetRawData()
    {
        if (m_pixels.empty()) {
            return nullptr;
        }
        return m_pixels[0].data.data();
    }

    const T* GetRawData() const
    {
        if (m_pixels.empty()) {
            return nullptr;
        }
        return m_pixels[0].data.data();
    }

    PixelType& GetPixel(uint32_t x, uint32_t y)
    {
        if (x >= m_width || y >= m_height) {
            throw std::out_of_range("Pixel coordinates out of bounds.");
        }
        return m_pixels[y * m_width + x];
    }

    const PixelType& GetPixel(uint32_t x, uint32_t y) const
    {
        if (x >= m_width || y >= m_height) {
            throw std::out_of_range("Pixel coordinates out of bounds.");
        }
        return m_pixels[y * m_width + x];
    }

    void Resize(uint32_t newWidth, uint32_t newHeight)
    {
        if (newWidth == 0 || newHeight == 0) {
            throw std::invalid_argument("Image dimensions must be positive.");
        }
        if (newWidth == m_width && newHeight == m_height) {
            return; // No change needed
        }
        m_width = newWidth;
        m_height = newHeight;
        m_pixels.resize(m_width * m_height);
    }

    void Clear(const PixelType& clearColor = PixelType())
    {
        std::fill(m_pixels.begin(), m_pixels.end(), clearColor);
    }

    // Function to convert to a different image type (e.g., 8-bit RGB to float RGB)
    template <typename TargetT, size_t TargetChannels = Channels>
    Image<TargetT, TargetChannels> Convert() const
    {
        Image<TargetT, TargetChannels> newImage(m_width, m_height);
        for (size_t i = 0; i < m_pixels.size(); ++i) {
            newImage.m_pixels[i] = m_pixels[i].template ConvertTo<TargetT>();
        }
        return newImage;
    }

private:
    size_t m_width;

    size_t m_height;

    std::vector<PixelType> m_pixels;
};
} // namespace prev::render::image

#endif
