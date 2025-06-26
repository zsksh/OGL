#pragma once
#include<vector>
#include<string>
//#include<renderer/rhi_struct.h>
namespace CGL {
	struct TextureData {
		int width;
		int height;
		int channels;
		void* pixels;
	};
	struct Color {
		float b, g, r, a;
		Color() = default;
		Color(float R, float G, float B, float A)
			: b(B), g(G), r(R), a(A) {}

	};

	class Texture2D final {
	public:
		Texture2D() = default;
		Texture2D(Color color) {
			m_width = 1;
			m_height = 1;
			m_channel = 4;
			m_pixels.resize(4);
			m_pixels[0] = color.r;
			m_pixels[1] = color.g;
			m_pixels[2] = color.b;
		}
		Color get_pixel(int x, int y) {
			return Color(m_pixels[m_channel * y * m_width + m_channel * x + 0],
				m_pixels[m_channel * y * m_width + m_channel * x + 1],
				m_pixels[m_channel * y * m_width + m_channel * x + 2], 255);
		}
		Texture2D(int width, int height) :m_width(width), m_height(height) {
			m_pixels.resize(m_height * m_width * 4, 0);
		}
		~Texture2D() = default;
		void initialize(TextureData& data);
		bool load_texture(const std::string& filepath);
		void update_from_raw_data(std::vector<uint32_t>&data);
		Color Sample2D(float a, float b) {
			int x = static_cast<int>(a * m_width - 0.5f) % m_width;
			int y = m_height - static_cast<int>(b * m_height - 0.5f) % m_height;
			x = x < 0 ? m_width + x : x;
			y = y < 0 ? m_height + y : y;
			x = std::min(m_width - 1, x);
			y = std::min(m_height - 1, y);

			return get_pixel(x, y);
		}
	public:
		std::vector<unsigned char> m_pixels;
		int m_width = 0;
		int m_height = 0;
		int m_channel = 4;
	};



}