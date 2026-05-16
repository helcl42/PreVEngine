#include "ImageFactory.h"
#include "Image.h"

#include "../../common/Logger.h"
#include "../../util/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

namespace prev::render::image {
namespace {
    template <typename T, size_t Channels>
    std::unique_ptr<Image<T, Channels>> CreateImageFromData(const uint32_t width, const uint32_t height, const uint8_t* data)
    {
        if (!data) {
            LOGE("Image: Failed to create image from data: data is null");
            return nullptr;
        }

        return std::make_unique<Image<T, Channels>>(width, height, data);
    }

    std::unique_ptr<IImage> CreateUint8ImageFromData(const uint32_t width, const uint32_t height, const uint32_t channelCount, const uint8_t* data)
    {
        switch (channelCount) {
        case 1:
            return CreateImageFromData<uint8_t, 1>(width, height, data);
        case 2:
            return CreateImageFromData<uint8_t, 2>(width, height, data);
        case 3:
            return CreateImageFromData<uint8_t, 3>(width, height, data);
        case 4:
            return CreateImageFromData<uint8_t, 4>(width, height, data);
        default:
            LOGE("Image: Unsupported channel count: %u", channelCount);
            return nullptr;
        }
    }
} // namespace

std::unique_ptr<IImage> ImageFactory::CreateImage(const std::string& filename, bool flipVertically) const
{
    if (!prev::util::file::Exists(filename)) {
        LOGE("Image: File not found: %s", filename.c_str());
        return nullptr;
    }

    LOGI("Loading image: %s...", filename.c_str());

    stbi_set_flip_vertically_on_load(flipVertically);
#ifdef TARGET_PLATFORM_IOS
    stbi_convert_iphone_png_to_rgb(true);
#endif

    const int ouputChannelCount{ STBI_rgb_alpha };
    int width, height, channelCount;
    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load(filename.c_str(), &width, &height, &channelCount, STBI_rgb_alpha));
    if (!imageBytes) {
        LOGE("Image: Failed to load image: %s", filename.c_str());
        return nullptr;
    }

    auto image = CreateUint8ImageFromData(width, height, ouputChannelCount, imageBytes);

    LOGI("Loaded image: %s (%dx%d|%d)", filename.c_str(), width, height, ouputChannelCount);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<IImage> ImageFactory::CreateImageFromMemory(const uint8_t* data, const uint32_t dataLength) const
{
    LOGI("Loading image from memory: %u bytes", dataLength);

#ifdef TARGET_PLATFORM_IOS
    stbi_convert_iphone_png_to_rgb(true);
#endif

    const int ouputChannelCount{ STBI_rgb_alpha };
    int width, height, channelCount;
    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load_from_memory(data, dataLength, &width, &height, &channelCount, ouputChannelCount));
    if (!imageBytes) {
        LOGE("Image: Failed to load image from memory");
        return nullptr;
    }

    auto image = CreateUint8ImageFromData(width, height, ouputChannelCount, imageBytes);

    LOGI("Loaded image from memory: %u bytes (%dx%d|%d)", dataLength, width, height, ouputChannelCount);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<IImage> ImageFactory::CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient, const uint32_t checkers) const
{
    auto image = std::make_unique<Image<uint8_t, 4>>(width, height);
    for (uint32_t y = 0; y < image->GetHeight(); ++y) {
        for (uint32_t x = 0; x < image->GetWidth(); ++x) {
            auto& pix{ image->GetPixel(x, y) };

            pix = { 0, 0, 0, 255 };

            if (gradient) {
                pix = { static_cast<uint8_t>(x), static_cast<uint8_t>(y), 0, 255 };
            }

            if (checkers) {
                if ((x / (width / checkers)) % 2 == (y / (height / checkers)) % 2) {
                    pix[2] = 128;
                }
            }
        }
    }

    return image;
}

std::unique_ptr<IImage> ImageFactory::CreateImageWithColor(const uint32_t width, const uint32_t height, const Pixel<uint8_t, 4>& color) const
{
    auto image = std::make_unique<Image<uint8_t, 4>>(width, height);
    image->Clear(color);
    return image;
}

std::unique_ptr<IImage> ImageFactory::CreateResizedImage(const IImage& source, const uint32_t newWidth, const uint32_t newHeight) const
{
    const uint32_t srcW = source.GetWidth();
    const uint32_t srcH = source.GetHeight();
    const uint32_t channels = source.GetChannels();
    const uint8_t* src = source.GetRawDataPtr();

    std::vector<uint8_t> resized(newWidth * newHeight * channels);
    for (uint32_t y = 0; y < newHeight; ++y) {
        for (uint32_t x = 0; x < newWidth; ++x) {
            const float srcX = static_cast<float>(x) * (srcW - 1) / static_cast<float>(newWidth - 1);
            const float srcY = static_cast<float>(y) * (srcH - 1) / static_cast<float>(newHeight - 1);

            const uint32_t x0 = static_cast<uint32_t>(srcX);
            const uint32_t y0 = static_cast<uint32_t>(srcY);
            const uint32_t x1 = std::min(x0 + 1, srcW - 1);
            const uint32_t y1 = std::min(y0 + 1, srcH - 1);

            const float fx = srcX - x0;
            const float fy = srcY - y0;

            for (uint32_t c = 0; c < channels; ++c) {
                const float v00 = src[(y0 * srcW + x0) * channels + c];
                const float v10 = src[(y0 * srcW + x1) * channels + c];
                const float v01 = src[(y1 * srcW + x0) * channels + c];
                const float v11 = src[(y1 * srcW + x1) * channels + c];

                const float value = v00 * (1.0f - fx) * (1.0f - fy) + v10 * fx * (1.0f - fy) + v01 * (1.0f - fx) * fy + v11 * fx * fy;
                resized[(y * newWidth + x) * channels + c] = static_cast<uint8_t>(std::min(value + 0.5f, 255.0f));
            }
        }
    }

    return CreateUint8ImageFromData(newWidth, newHeight, channels, resized.data());
}
} // namespace prev::render::image
