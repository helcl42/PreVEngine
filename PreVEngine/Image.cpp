#include "Image.h"
#include "Validation.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

#include <fstream>

namespace PreVEngine
{
	RGBA::RGBA()
		: R(0), G(0), B(0), A(255)
	{
	}

	RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		: R(r), G(g), B(b), A(a)
	{
	}

	void RGBA::Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	RGBA RGBA::Lerp(const RGBA& c, const float f)
	{
		return RGBA(uint8_t(f * (c.R - R) + R), uint8_t(f * (c.G - G) + G), uint8_t(f * (c.B - B) + B), uint8_t(f * (c.A - A) + A));
	}



	Image::Image()
	{
	}

	Image::Image(const int width, const int height)
	{
		SetSize(width, height);
	}

	Image::Image(const int width, const int height, const uint8_t* bytes)
	{
		SetSize(width, height);

		std::memcpy(m_buffer, bytes, size_t(width * height * sizeof(RGBA)));
	}

	Image::Image(const int width, const int height, const RGBA* pixels)
	{
		SetSize(width, height);

		std::memcpy(m_buffer, pixels, size_t(width * height * sizeof(RGBA)));
	}

	Image::~Image()
	{
		CleanUp();
	}

	Image::Image(const Image& other)
	{
		size_t size{ other.m_width * other.m_height * sizeof(RGBA) };
		m_buffer = new RGBA[size];
		std::memcpy(m_buffer, other.m_buffer, size);

		m_width = other.m_width;
		m_height = other.m_height;
	}

	Image& Image::operator=(const Image& other)
	{
		if (this != &other)
		{
			CleanUp();

			size_t size{ other.m_width * other.m_height * sizeof(RGBA) };
			m_buffer = new RGBA[size];
			std::memcpy(m_buffer, other.m_buffer, size);

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
		if (this != &other)
		{
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
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = nullptr;
		}
	}

	void Image::SetSize(const int w, const int h)
	{
		CleanUp();

		m_width = w;
		m_height = h;
		m_buffer = new RGBA[w * h * sizeof(RGBA)];
	}

	RGBA* Image::GetBuffer() const
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
		std::memset(m_buffer, 0, size_t(m_width * m_height * sizeof(RGBA)));
	}

	void Image::Clear(const RGBA& color)
	{
		for (uint32_t i = 0; i < m_width * m_height; i++)
		{
			m_buffer[i] = color;
		}
	}

	bool ImageFactory::FileExists(const std::string& fileName)
	{
#ifdef ANDROID
		return true;
#else
		std::ifstream infile(fileName);
		return infile.good();
#endif
	}

	std::shared_ptr<Image> ImageFactory::CreateImage(const std::string& filename, bool flipVertically) const
	{
		if (!FileExists(filename))
		{
			LOGE("Image: File not found: %s\n", filename.c_str());
			return nullptr;
		}

		int w, h, n;
		stbi_set_flip_vertically_on_load(flipVertically);

		uint8_t* imageBytes = (uint8_t*)stbi_load(filename.c_str(), &w, &h, &n, sizeof(RGBA));
		if (!imageBytes)
		{
			LOGE("Image: Failed to load texture: %s", filename.c_str());
			return nullptr;
		}

		LOGI("Load image: %s (%dx%d)\n", filename.c_str(), w, h);

		std::shared_ptr<Image> image = std::make_shared<Image>(w, h, imageBytes);

		stbi_image_free(imageBytes);

		return image;
	}

	std::shared_ptr<Image> ImageFactory::CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient, const int checkers) const
	{
		std::shared_ptr<Image> image = std::make_shared<Image>(width, height);
		if (image == nullptr)
		{
			return image;
		}

		RGBA* buffer = image->GetBuffer();

		for (uint32_t y = 0; y < image->GetHeight(); ++y)
		{
			for (uint32_t x = 0; x < image->GetWidth(); ++x)
			{
				RGBA* pix = &buffer[x + y * width];

				*pix = { 0, 0, 0, 255 };

				if (gradient)
				{
					*pix = { uint8_t(x), uint8_t(y), 0, 255 };
				}

				if (checkers)
				{
					if ((x / (width / checkers)) % 2 == (y / (height / checkers)) % 2)
					{
						pix->B = 128;
					}
				}
			}
		}

		return image;
	}

	std::shared_ptr<Image> ImageFactory::CreateImageWithColor(const uint32_t width, const uint32_t height, const RGBA& color) const
	{
		std::shared_ptr<Image> image = std::make_shared<Image>(width, height);
		if (image == nullptr)
		{
			return image;
		}

		image->Clear(color);

		return image;
	}
}
