#include "Image.h"
#include "Validation.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

#include <fstream>

namespace PreVEngine {
RGBA::RGBA()
    : R(0)
    , G(0)
    , B(0)
    , A(255)
{
}

RGBA::RGBA(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    : R(r)
    , G(g)
    , B(b)
    , A(a)
{
}

void RGBA::Set(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    R = r;
    G = g;
    B = b;
    A = a;
}

RGBA RGBA::Lerp(const RGBA& c, const float f)
{
    return RGBA(static_cast<uint8_t>(f * (c.R - R) + R), static_cast<uint8_t>(f * (c.G - G) + G), static_cast<uint8_t>(f * (c.B - B) + B), static_cast<uint8_t>(f * (c.A - A) + A));
}

Image::Image()
    : m_width(0)
    , m_height(0)
    , m_buffer(nullptr)
{
}

Image::Image(const int width, const int height)
{
    SetSize(width, height);
}

Image::Image(const int width, const int height, const uint8_t* bytes)
{
    SetSize(width, height);

    std::memcpy(m_buffer.get(), bytes, size_t(width * height * sizeof(RGBA)));
}

Image::Image(const int width, const int height, const RGBA* pixels)
{
    SetSize(width, height);

    std::memcpy(m_buffer.get(), pixels, size_t(width * height * sizeof(RGBA)));
}

Image::~Image()
{
    CleanUp();
}

Image::Image(const Image& other)
{
    const size_t size{ other.m_width * other.m_height };
    m_buffer = std::shared_ptr<RGBA[]>(new RGBA[size]);
    std::memcpy(m_buffer.get(), other.m_buffer.get(), size * sizeof(RGBA));

    m_width = other.m_width;
    m_height = other.m_height;
}

Image& Image::operator=(const Image& other)
{
    if (this != &other) {
        CleanUp();

        const size_t size{ other.m_width * other.m_height };
        m_buffer = std::shared_ptr<RGBA[]>(new RGBA[size]);
        std::memcpy(m_buffer.get(), other.m_buffer.get(), size * sizeof(RGBA));

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
    m_buffer = nullptr;
}

void Image::SetSize(const int w, const int h)
{
    CleanUp();

    m_width = w;
    m_height = h;
    m_buffer = std::shared_ptr<RGBA[]>(new RGBA[w * h]);
}

std::shared_ptr<RGBA[]> Image::GetBuffer() const
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
    const size_t size(m_width * m_height);
    std::memset(m_buffer.get(), 0, size * sizeof(RGBA));
}

void Image::Clear(const RGBA& color)
{
    for (uint32_t i = 0; i < m_width * m_height; i++) {
        m_buffer[i] = color;
    }
}

bool ImageFactory::FileExists(const std::string& fileName)
{
#if defined(__ANDROID__)
    AAsset* asset = android_open_asset(modelPath.c_str(), AASSET_MODE_STREAMING);
    bool result = !!asset;
    AAsset_close(asset);
    return result;
#else
    std::ifstream infile(fileName);
    return infile.good();
#endif
}

std::unique_ptr<Image> ImageFactory::CreateImage(const std::string& filename, bool flipVertically) const
{
    if (!FileExists(filename)) {
        LOGE("Image: File not found: %s\n", filename.c_str());
        return nullptr;
    }

    int w, h, n;
    stbi_set_flip_vertically_on_load(flipVertically);

    uint8_t* imageBytes = (uint8_t*)stbi_load(filename.c_str(), &w, &h, &n, sizeof(RGBA));
    if (!imageBytes) {
        LOGE("Image: Failed to load texture: %s", filename.c_str());
        return nullptr;
    }

    LOGI("Load image: %s (%dx%d)\n", filename.c_str(), w, h);

    auto image = std::make_unique<Image>(w, h, imageBytes);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<Image> ImageFactory::CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient, const int checkers) const
{
    auto image = std::make_unique<Image>(width, height);
    if (image == nullptr) {
        return image;
    }

    auto buffer = image->GetBuffer();

    for (uint32_t y = 0; y < image->GetHeight(); ++y) {
        for (uint32_t x = 0; x < image->GetWidth(); ++x) {
            auto pix = &buffer[y * width + x];

            *pix = { 0, 0, 0, 255 };

            if (gradient) {
                *pix = { static_cast<uint8_t>(x), static_cast<uint8_t>(y), 0, 255 };
            }

            if (checkers) {
                if ((x / (width / checkers)) % 2 == (y / (height / checkers)) % 2) {
                    pix->B = 128;
                }
            }
        }
    }

    return image;
}

std::unique_ptr<Image> ImageFactory::CreateImageWithColor(const uint32_t width, const uint32_t height, const RGBA& color) const
{
    auto image = std::make_unique<Image>(width, height);
    if (image == nullptr) {
        return image;
    }

    image->Clear(color);

    return image;
}
} // namespace PreVEngine
