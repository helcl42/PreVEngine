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

    int OverrideDesiredChannelCount(int c) {
#ifdef TARGET_PLATFORM_ANDROID
        c = STBI_rgb_alpha;
#endif
        return c;
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

    int w, h, c;
    if (!stbi_info(filename.c_str(), &w, &h, &c)) {
        LOGE("Image: Failed to get image info for: %s", filename.c_str());
        return nullptr;
    }
    c = OverrideDesiredChannelCount(c);

    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load(filename.c_str(), &w, &h, &c, c));
    if (!imageBytes) {
        LOGE("Image: Failed to load texture: %s", filename.c_str());
        return nullptr;
    }
    c = OverrideDesiredChannelCount(c);

    auto image = CreateUint8ImageFromData(w, h, c, imageBytes);

    LOGI("Loaded image: %s (%dx%d|%d)", filename.c_str(), w, h, c);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<IImage> ImageFactory::CreateImageFromMemory(const uint8_t* data, const uint32_t dataLength) const
{
    LOGI("Loading image from memory: %u bytes", dataLength);

#ifdef TARGET_PLATFORM_IOS
    stbi_convert_iphone_png_to_rgb(true);
#endif

    int w, h, c;
    if (!stbi_info_from_memory(data, dataLength, &w, &h, &c)) {
        LOGE("Image: Failed to get image info for image in memory with size %u", dataLength);
        return nullptr;
    }
    c = OverrideDesiredChannelCount(c);

    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load_from_memory(data, dataLength, &w, &h, &c, c));
    if (!imageBytes) {
        LOGE("Image: Failed to load texture from memory");
        return nullptr;
    }
    c = OverrideDesiredChannelCount(c);

    auto image = CreateUint8ImageFromData(w, h, c, imageBytes);

    LOGI("Loaded image from memory: %u bytes (%dx%d|%d)", dataLength, w, h, c);

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
} // namespace prev::render::image
