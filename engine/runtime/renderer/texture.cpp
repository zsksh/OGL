#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include<iostream>
#include<renderer/texture.h>
//#include<renderer/scene_data.h>
namespace CGL {
	bool Texture2D::load_texture(const std::string& filepath) {
		//stbi_set_flip_vertically_on_load(true);
		auto raw = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channel, 0);
		if (raw == nullptr) {
			std::cerr << "Failed to load image from " << filepath << std::endl;
			exit(1);
		}
		m_pixels.resize(m_height * m_width * 4);
		for (int x = 0; x < m_height * m_width; x++) {
			unsigned char& r = m_pixels[x * 4 + 0];
			unsigned char& g = m_pixels[x * 4 + 1];
			unsigned char& b = m_pixels[x * 4 + 2];
			unsigned char& a = m_pixels[x * 4 + 3];
			int fromIndex = x * m_channel;
			switch (m_channel) {
			case 1:
				r = g = b = raw[fromIndex], a = 255;
				break;
			case 3:
				r = raw[fromIndex], g = raw[fromIndex + 1], b = raw[fromIndex + 2], a = 255;
				break;
			case 4:
				r = raw[fromIndex], g = raw[fromIndex + 1], b = raw[fromIndex + 2], a = raw[fromIndex + 3];
				break;
			default:
				r = g = b = raw[fromIndex], a = 255;
				break;
			}
		}
		m_channel = 4;
	}

	void Texture2D::initialize(TextureData& data) {
		m_width = data.width;
		m_height = data.height;
		unsigned char* raw = static_cast<unsigned char*>(data.pixels);
		m_pixels.resize(m_height * m_width * 4);
		for (int x = 0; x < m_height * m_width; x++) {
			unsigned char& r = m_pixels[x * 4 + 0];
			unsigned char& g = m_pixels[x * 4 + 1];
			unsigned char& b = m_pixels[x * 4 + 2];
			unsigned char& a = m_pixels[x * 4 + 3];
			int fromIndex = x * m_channel;
			switch (m_channel) {
			case 1:
				r = g = b = raw[fromIndex], a = 255;
				break;
			case 3:
				r = raw[fromIndex], g = raw[fromIndex + 1], b = raw[fromIndex + 2], a = 255;
				break;
			case 4:
				r = raw[fromIndex], g = raw[fromIndex + 1], b = raw[fromIndex + 2], a = raw[fromIndex + 3];
				break;
			default:
				r = g = b = raw[fromIndex], a = 255;
				break;
			}
		}
		m_channel = 4;
	}
	void Texture2D::update_from_raw_data(std::vector<uint32_t>& data) {
		size_t total_bytes = m_width * m_height * 4;
		if (m_pixels.size() != total_bytes) {
			m_pixels.resize(total_bytes);
		}    
		for (size_t i = 0; i < m_width * m_height; ++i) {

			size_t source_index = i * 4;

			// Read the four separate byte components from the source data.
			unsigned char r = data[source_index + 0];
			unsigned char g = data[source_index + 1];
			unsigned char b = data[source_index + 2];
			unsigned char a = data[source_index + 3];

			//uint32_t packed_color = (a << 24) | (b << 16) | (g << 8) | r;
			m_pixels[4 * i + 0] = r;
			m_pixels[4 * i + 1] = g;
			m_pixels[4 * i + 2] = b;
			//m_pixels[4 * i + 3] = a;

		}
	}



}
