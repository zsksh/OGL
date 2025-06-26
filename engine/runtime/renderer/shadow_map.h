#pragma once
#include<vector>
namespace CGL {
	class Framebuffer;
	class ShadowMap {
	public:
		ShadowMap(int width, int height) : m_width(width), m_height(height) {
			m_depth_buffer.resize(width * height, 1);// opengl default is 1;
		}
		void clear() {
			m_depth_buffer.clear();
			m_height = m_width = 0;
		}
		void initialize(Framebuffer* frambuffer);
		float getSample(float u, float v) const {
			int x_coord = std::min((int)(u * m_width), m_width - 1);
			int y_coord = std::min((int)(v * m_height), m_height - 1);
			x_coord = std::max(x_coord, 0);
			y_coord = std::max(y_coord, 0);
			return m_depth_buffer[y_coord * m_width + x_coord];
		}

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }
	private:
		std::vector<float> m_depth_buffer;
		int m_width;
		int m_height;
	};




}