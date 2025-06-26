#pragma once
#include<renderer/renderpass.h>
#include<renderer/command_buffer.h>
#include<renderer/render_system.h>
namespace CGL {
	class RenderSystem;
	class LightingPass : public RenderPass {
		using Shading_ptr = std::shared_ptr<GraphicsPipeline>;
		//struct LitSceneUniforms {
		//	mat4 view;
		//	mat4 projection;
		//	mat4 light_space_matrix;
		//	std::shared_ptr<ShadowMap> shadow_map;
		//	std::vector<std::shared_ptr<Light>> lights;
		//};
	public:
		LightingPass(std::shared_ptr<GraphicsPipeline> pipeline)
			: RenderPass("LightingPass"), m_pipeline(pipeline) {}
		void execute(RenderSystem& renderer, RenderContext& context) override {
			auto& camera = renderer.getMainCamera();
			auto& off_screen_buffer = renderer.getOffScreenBuffer();


			renderer.set_viewport(off_screen_buffer->m_width, off_screen_buffer->m_height);
			auto scene = context.scene;
			m_framebuffer = std::make_shared<Framebuffer>(renderer.width(), renderer.height());
				//renderer.getFramebuffer();// use main buffer
			//renderer.clearColor(0.1f, 0.2f, 0.2f, 1.0f);
			renderer.setGraphicsPipelie(m_pipeline);
			renderer.setCurrentFrambuffer(m_framebuffer);
			auto& light = context.scene->getLights()[0];// first light;
			
			mat4 view = camera->get_view_matrix();// cgl_look_at(vec3(0.9, 0.5, 3.0), vec3(0, 0, 0), vec3(0, 1, 0));
			mat4 projection = camera->get_projection_matrix();//cgl_perspective(glm::radians(45.f), 1.0f, 0.1f, 100.f);
			SceneUniforms su;
			mat4 lview = cgl_look_at(light->getPosition(), vec3(0, 0, 0), vec3(0, 1, 0));
			mat4 lprojection = cgl_orth(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
			su.light_space_matrix = lprojection * lview;
			su.view_pos = vec3(0.9, 0.5, 3.0);
			
			su.lights.assign(scene->getLights().begin(), scene->getLights().end());
			su.shadow_map = context.shadow_map;
			su.projection = projection;
			su.view = view;
			for (const auto& object : scene->getObjects()) {
				auto static_mesh = object.mesh;
				auto& mv = static_mesh->vertices;
				auto& mf = static_mesh->indices;
				ObjectUniforms ou;
				ou.model = object.transform->get_matrix();
				renderer.draw(mv, mf, ou, su, *(object.material));
			}
			context.final_color_buffer = 
				std::make_shared<std::vector<uint32_t>>(m_framebuffer->m_color_buffer);
			context.lit_scene_tex = m_framebuffer->getColorTexture();
		}
		void execute(RenderSystem& renderer, const Scene& scene, const Camera& camera) override {

		}
	private:
		Shading_ptr m_pipeline;
	};
}