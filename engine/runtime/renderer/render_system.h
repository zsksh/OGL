#pragma once
#include<memory>
#include<vector>
#include<renderer/rasterizer.h>
#include<renderer/framebuffer.h>
#include<renderer/graphics_pipeline.h>
#include<renderer/scene.h>
#include<renderer/camera.h>
namespace CGL {
	class WindowCreateInfo;
	class RenderPass;
	class RenderSystem {
		using VertexBuffer = std::vector<Vertex>;
		using Shading_ptr = std::shared_ptr<GraphicsPipeline>;
	public:
		RenderSystem() {}
		void set_viewport(int width, int height);

		void clearColor(float r, float g, float b, float a);
		void clearDepth();
		void initialize(WindowCreateInfo info);
		void tick();
		void draw_indexed(VertexData& v0, VertexData& v1, VertexData& v2);
		
		void draw(VertexBuffer& vertices, std::vector<unsigned>& indices,
			ObjectUniforms& obj, SceneUniforms& scene, Material& material);
		void setGraphicsPipelie(Shading_ptr pipeline) {
			m_pipeline = pipeline;
		}
		void setCurrentFrambuffer(std::shared_ptr<Framebuffer> framebuffer) {
			m_active_framebuffer = framebuffer;
		}
		void setMainCamera(std::shared_ptr<Camera> camera) {
			m_main_camera = camera;
		}
		void setScene(std::shared_ptr<Scene> scene) { m_scene = scene; }
		std::shared_ptr<Framebuffer> getFramebuffer() { return m_framebuffer; }
		void addPass(std::shared_ptr<RenderPass> pass) {
			m_render_pass.push_back(pass);
		}
		void blit_to_screen(std::vector<uint32_t>& final_color_buffer) {
			m_framebuffer->m_color_buffer.assign(final_color_buffer.begin(),
				final_color_buffer.end()
			);
		}
		void prepareContext(RenderContext& context) {
			context.scene = m_scene;
		}
		std::shared_ptr<Camera> getMainCamera() {
			return m_main_camera;
		}
		std::shared_ptr<Framebuffer> getOffScreenBuffer() { return m_framebuffer; }
		int width() { return m_width; }
		int height() { return m_height; }
	private:
		mat4 m_viewport_matrix;
	private:
		std::unique_ptr<Rasterizer>   m_rasterizer;
		std::shared_ptr<Framebuffer>  m_framebuffer;
		std::shared_ptr<GraphicsPipeline> m_pipeline;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<Framebuffer>  m_active_framebuffer;
		std::vector<std::shared_ptr<RenderPass>> m_render_pass;
		std::shared_ptr<Camera> m_main_camera;
		int m_width=1000;
		int m_height=1000;
	};
	

}