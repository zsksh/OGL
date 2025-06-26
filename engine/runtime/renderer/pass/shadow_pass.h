#include<renderer/renderpass.h>
namespace CGL {
	class ShadowPass : public RenderPass {
		using Shading_ptr = std::shared_ptr<GraphicsPipeline>;
		//struct ShadowPassUniforms { 
		//	mat4 view;
		//	mat4 projection;
		//	mat4 light_space_matrix;
		//	std::shared_ptr<ShadowMap> shadow_map;
		//	std::vector<std::shared_ptr<Light>> lights;
		//};
	public:

		ShadowPass(int resolution,std::shared_ptr<GraphicsPipeline> pipeline)
			: RenderPass("LightingPass"), m_shadow_pipeline(pipeline) {
			m_resolution = resolution;
			m_framebuffer = std::make_shared<Framebuffer>(resolution, resolution);
		}
		

		void execute(RenderSystem& renderer, RenderContext& context) override {
			m_framebuffer = std::make_shared<Framebuffer>(m_resolution, m_resolution);
			auto scene = context.scene;
			renderer.set_viewport(m_resolution, m_resolution);
			renderer.clearColor(0.1f, 0.2f, 0.2f, 1.0f);
			renderer.setGraphicsPipelie(m_shadow_pipeline);
			renderer.setCurrentFrambuffer(m_framebuffer);
			auto& light = context.scene->getLights()[0];// first light;
			mat4 view = cgl_look_at(light->getPosition() , vec3(0, 0, 0), vec3(0, 1, 0));
			//std::cout <<"shadow: "<< light->getPosition()[0]<<" "<<
			//	light->getPosition()[1] <<" "<< light->getPosition()[2]<< std::endl;
			mat4 projection = cgl_orth(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
			SceneUniforms su;
			su.light_space_matrix = projection * view;
			for (const auto& object : scene->getObjects()) {
 				auto static_mesh = object.mesh;
				auto& mv = static_mesh->vertices;
				auto& mf = static_mesh->indices;
				ObjectUniforms ou;
				ou.model = object.transform->get_matrix();
				renderer.draw(mv, mf, ou, su, *(object.material));
			}

			// output
			context.final_color_buffer =
				std::make_shared<std::vector<uint32_t>>(m_framebuffer->m_color_buffer);

			auto shadow_map = std::make_shared<ShadowMap>(m_resolution, m_resolution);
			shadow_map->initialize(m_framebuffer.get());
			context.shadow_map = shadow_map;
		}

		void execute(RenderSystem& renderer, const Scene& scene, const Camera& camera) override {
		    auto viewport = cgl_viewport(0, 0, m_resolution, m_resolution);
			renderer.clearColor(0.1f, 0.2f, 0.2f, 1.0f);
			renderer.setGraphicsPipelie(m_shadow_pipeline);
			renderer.setCurrentFrambuffer(m_framebuffer);


			//auto depth_frame = new Framebuffer(width, height);
			//.//std::shared_ptr<Framebuffer> depth_frame =
			//	std::make_shared<Framebuffer>(width, height);
		//	m_active_framebuffer = depth_frame;
			//auto& shadow_vertex = std::make_shared<ShadowVertexShader>();
			//auto& shadow_frag = std::make_shared<ShadowFragmentShader>();
			//m_pipeline = std::make_unique<GraphicsPipeline>(shadow_vertex, shadow_frag);
			//auto& vs = m_pipeline->get_vertex_shader();
			//auto& fs = m_pipeline->get_fragment_shader();
			static mat4 view = cgl_look_at(vec3(0.0f, 4.0f, 4.0f), vec3(0, 0, 0), vec3(0, 1, 0));
			static mat4 projection = cgl_orth(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
			SceneUniforms su;
			su.light_space_matrix = projection * view;
			for (const auto& object : scene.getObjects()) {
				auto static_mesh = object.mesh;
				auto& mv = static_mesh->vertices;
				auto& mf = static_mesh->indices;
				ObjectUniforms ou;
				ou.model = object.transform->get_matrix();
				renderer.draw(mv, mf, ou, su, *(object.material));
			}




		}
	private:
		int m_resolution;
		Shading_ptr m_shadow_pipeline;
		//Framebuffer  framebuffer;
		//const Color* m_input_gbuffer = nullptr;
	};
}