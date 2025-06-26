#include<renderer/framebuffer.h>
#include<omp.h>
namespace CGL {
	void Framebuffer::resolve() {
		for (int y = 0; y < m_height; ++y) {
			for (int x = 0; x < m_width; ++x) {
				float r = 0, g = 0, b = 0;
				size_t pixel_index = y * m_width + x;
				//const auto& color_samples = _multisampleColorTarget->getRawBuffer()[pixel_index];

				//for (int i = 0; i < m_sample_count; ++i) {
				//	r += color_samples[i].r;
				//	g += color_samples[i].g;
				//	b += color_samples[i].b;
				//}
				//_resolvedColorTarget->setPixel(x, y, Color(r / _sample_count, g / _sample_count, b / _sample_count));
			}
		}
	}


	void Framebuffer::write_color(unsigned x, unsigned y) {
		if (x >= m_width || y >= m_height) return;
		//if(m_depth_buffer[y*m_width+x])
		m_color_buffer[y * m_width * 4 + 4 * x + 0] = 150;
		m_color_buffer[y * m_width * 4 + 4 * x + 1] = 40;
		m_color_buffer[y * m_width * 4 + 4 * x + 2] = 30;
		m_color_buffer[y * m_width * 4 + 4 * x + 3] = 255;
	}

	void Framebuffer::write_color(unsigned x, unsigned y, float depth) {
		if (x >= m_width || y >= m_height) return;
		//#pragma omp critical
		{
			if (m_depth_buffer[y * m_width + x] > depth) {
				m_depth_buffer[y * m_width + x] = depth;
				m_color_buffer[y * m_width * 4 + 4 * x + 0] = m_color[0];
				m_color_buffer[y * m_width * 4 + 4 * x + 1] = m_color[1];
				m_color_buffer[y * m_width * 4 + 4 * x + 2] = m_color[2];
				m_color_buffer[y * m_width * 4 + 4 * x + 3] = 255;
			}
		}

	}
	void Framebuffer::write_color(unsigned x, unsigned y, float depth, FragmentData& frag) {
		if (x >= m_width || y >= m_height) return;
		//auto u = frag.m_tex[0];
		//auto v = frag.m_tex[1];
		//auto color = m_texture->Sample2D(u, v);
		//#pragma omp critical
		{
			if (m_depth_buffer[y * m_width + x] > depth) {
				m_depth_buffer[y * m_width + x] = depth;
				//m_color_buffer[y * m_width * 4 + 4 * x + 0] = color.r;
				//m_color_buffer[y * m_width * 4 + 4 * x + 1] = color.g;
				//m_color_buffer[y * m_width * 4 + 4 * x + 2] = color.b;
				//m_color_buffer[y * m_width * 4 + 4 * x + 3] = 255;
			}
		}
	}

	void Framebuffer::write_color(unsigned x, unsigned y, float depth, const Color& color) {
		if (x >= m_width || y >= m_height) return;

		///printf("Framebuffer::write_color called with coordinates: x=%d, y=%d\n", x, y);


		// --- Part 2: POKE a fixed memory location ---
		// We IGNORE the x and y from the rasterizer and write to a hardcoded
		// location (e.g., 50, 50) to see if this framebuffer is even visible.
		//int fixed_x = 50;
		//int fixed_y = 50;
		//int index = (fixed_y * m_width + fixed_x) * 4;
		//auto u = frag.m_tex[0];
		//auto v = frag.m_tex[1];
		//auto color = m_texture->Sample2D(u, v);
		//#pragma omp critical
		{
			if (m_depth_buffer[y * m_width + x] > depth) {
				m_depth_buffer[y * m_width + x] = depth;
				m_color_buffer[y * m_width * 4 + 4 * x + 0] = color.r;
				m_color_buffer[y * m_width * 4 + 4 * x + 1] = color.g;
				m_color_buffer[y * m_width * 4 + 4 * x + 2] = color.b;
				m_color_buffer[y * m_width * 4 + 4 * x + 3] = 255;
			}
		}
	}

	std::shared_ptr<Texture2D> Framebuffer::getColorTexture() {
		auto texture = std::make_shared<Texture2D>(m_width, m_height);
		texture->update_from_raw_data(m_color_buffer);
		return texture;
	}





}