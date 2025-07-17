
#ifndef __PIXEL_H__
#define __PIXEL_H__

#include <array>
#include <stdexcept>

namespace prev::render::image {

template <typename T, size_t Channels>
struct Pixel {
    Pixel()
    {
        data.fill(T{});
    }

    Pixel(const T& val)
    {
        data.fill(val);
    }

    Pixel(const std::initializer_list<T>& components)
    {
        if (components.size() > Channels) {
            throw std::invalid_argument("Initializer list size > number of channels");
        }

        size_t i = 0;
        for (const T& comp : components) {
            if (i < Channels) {
                data[i++] = comp;
            }
        }
        for (; i < Channels; ++i) {
            data[i] = T{};
        }
    }

    Pixel(const T* data)
    {
        if (data == nullptr) {
            throw std::invalid_argument("Pixel data cannot be null");
        }

        for (size_t i = 0; i < Channels; ++i) {
            this->data[i] = data[i];
        }
    }

    T& operator[](size_t index)
    {
        if (index >= Channels) {
            throw std::out_of_range("Pixel component index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const
    {
        if (index >= Channels) {
            throw std::out_of_range("Pixel component index out of range");
        }
        return data[index];
    }

    Pixel Lerp(const Pixel<T, Channels>& p, const float f) const
    {
        Pixel<T, Channels> newPixel;
        for (size_t i = 0; i < Channels; ++i) {
            newPixel.data[i] = static_cast<T>(f * (p.data[i] - data[i]) + data[i]);
        }
        return newPixel;
    }

    template <typename U>
    Pixel<U, Channels> ConvertTo() const
    {
        Pixel<U, Channels> newPixel;
        for (size_t i = 0; i < Channels; ++i) {
            newPixel.data[i] = static_cast<U>(data[i]);
        }
        return newPixel;
    }

    std::array<T, Channels> data;
};
} // namespace prev::render::image

#endif