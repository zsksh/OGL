#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include<string>
#include<core/math.h>
#include<resource/assetmanager.h>
#include<renderer/render_system.h>
#include<renderer/renderpass.h>
#include<renderer/pass/main_pass.h>
#include<renderer/pass/shadow_pass.h>
#include<renderer/pass/post_process_pass.h>
#include<renderer/texture.h>
#include<platform/window_system.h>
namespace CGL {
	inline bool save(Framebuffer& buffer, const std::string& filename) {
		const int width = buffer.m_width;
		const int height = buffer.m_height;
		const int channels = 4; // RGBA
		std::vector<uint8_t> pixelData(width * height * channels);
		for (size_t i = 0; i < buffer.m_color_buffer.size(); ++i) {
			pixelData[i] = buffer.m_color_buffer[i];
		}
		stbi_flip_vertically_on_write(true);
		if (stbi_write_png(filename.c_str(), width, height, channels, pixelData.data(), width * channels)) {
			return true;
		}
	}
	inline bool save_tex(Texture2D& buffer, const std::string& filename) {
		const int width = buffer.m_width;
		const int height = buffer.m_height;
		const int channels = 4; // RGBA
		// Convert from uint32_t (0xAABBGGRR) to bytes (RR GG BB AA)
		std::vector<uint8_t> pixelData(width * height * channels);
		for (size_t i = 0; i < height; i++) {
			for (size_t j = 0; j < width; j++) {
				pixelData[4 * i * width + 4 * j + 0] = buffer.get_pixel(j, i).r;
				pixelData[4 * i * width + 4 * j + 1] = buffer.get_pixel(j, i).g;
				pixelData[4 * i * width + 4 * j + 2] = buffer.get_pixel(j, i).b;
				pixelData[4 * i * width + 4 * j + 3] = buffer.get_pixel(j, i).a;
			}
		}
		stbi_flip_vertically_on_write(true);
		if (stbi_write_png(filename.c_str(), width, height, channels, pixelData.data(), width * channels)) {
			return true;
		}
	}

	void RenderSystem::initialize(WindowCreateInfo info) {
		m_width = info.width;
		m_height = info.height;
		m_rasterizer = std::make_unique<Rasterizer>();
		m_framebuffer = std::make_unique<Framebuffer>(m_width, m_height);
	}
	

	void RenderSystem::tick() {
		clearColor(0.2f, 0.3f, 0.3f, 1.0f);
		clearDepth();
		m_scene->update();
		RenderContext context;
		prepareContext(context);
		for (auto& pass : m_render_pass) {
			pass->execute(*this, context);
		}
		blit_to_screen(*(context.final_color_buffer));
		static int mm = 0;
		std::string filename = "../../../result/buffer" + std::to_string(mm++) + ".png";
		//save(*m_framebuffer, filename);
		//exit(0);
	}
	void RenderSystem::draw(VertexBuffer& vertices, std::vector<unsigned>& indices,
		ObjectUniforms& obj, SceneUniforms& scene, Material& material) {
		auto buffer_width = m_active_framebuffer->m_width;
		auto buffer_height = m_active_framebuffer->m_height;

		//scene.near_plane = 0.1f;
		//scene.far_plane = 100.0f;
		auto vertex_count = vertices.size();
		auto index_count = indices.size() / 3;
		auto vs = m_pipeline->get_vertex_shader();
		auto fs = m_pipeline->get_fragment_shader();
		auto& fun = [&](const FragmentData& frag) {
			Color final_color = fs->shade(frag, material, scene);
			m_active_framebuffer->write_color(frag.m_spos.x, frag.m_spos.y, frag.m_depth, final_color);
		};

		for (int x = 0; x < indices.size() / 3; x++) {
			VertexData v[3];
			int mm = 0;
			for (auto& sv : v) {
				sv.m_pos = vertices[indices[3 * x + mm]].position;
				sv.m_nor = vertices[indices[3 * x + mm]].normal;
				sv.m_tex = vertices[indices[3 * x + mm]].tex_coords;
				sv.vertex_id = vertices[indices[3 * x + mm]].vertex_id;
				mm++;
				vs->shade(sv, obj, scene);
				VertexData::pre_persp_correction(sv);
				sv.m_cpos = sv.m_cpos / sv.m_cpos[3];
				sv.m_spos = vec3(m_viewport_matrix * sv.m_cpos);
			}
			m_rasterizer->rasterizeTriangle(v[0], v[1], v[2],
				m_width, m_height, fun);
		}
	}


	void RenderSystem::draw_indexed(VertexData& v0, VertexData& v1, VertexData& v2) {
		//m_rasterizer->rasterizeTriangle(v0, v1, v2, 800, 800);
	}

	void RenderSystem::clearColor(float r, float g, float b, float a) {
		auto& color_buffer = m_framebuffer->m_color_buffer;
		for (int x = 0; x < color_buffer.size() / 4; x++) {
			color_buffer[4 * x + 0] = r * 255;
			color_buffer[4 * x + 1] = g * 255;
			color_buffer[4 * x + 2] = b * 255;
			color_buffer[4 * x + 3] = a * 255;
		}
	}
	void RenderSystem::clearDepth() {
		auto& depth = m_framebuffer->m_depth_buffer;
		std::fill(depth.begin(), depth.end(), 1.0f);
	}



	void RenderSystem::set_viewport(int width, int height) {
		m_width = width;   // <-- UNCOMMENT THIS
		m_height = height; // <-- UNCOMMENT THIS
		m_viewport_matrix = cgl_viewport(0, 0, width, height);

	}
}