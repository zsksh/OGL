#include<renderer/renderpass.h>
namespace CGL {
    class PostProcessPass : public RenderPass {
    public:
        PostProcessPass(std::shared_ptr<GraphicsPipeline> pipeline) : RenderPass("PostProcessPass") {
            m_pipeline = pipeline;  
        }

        void execute(RenderSystem& renderer, RenderContext& context) override {
            auto scene_texture = context.lit_scene_tex;
            assert(scene_texture != nullptr && "PostProcessPass requires a scene texture!");
            auto scene = context.scene;
            m_framebuffer = renderer.getFramebuffer();// use main buffer
            renderer.set_viewport(m_framebuffer->m_width, m_framebuffer->m_height);
            renderer.clearColor(0.1f, 0.2f, 0.2f, 1.0f);
            renderer.setGraphicsPipelie(m_pipeline);
            renderer.setCurrentFrambuffer(m_framebuffer);
            SceneUniforms ppu;

            ppu.scene_texture = scene_texture;
            std::vector<Vertex> dummy_verts;
            dummy_verts.resize(3);
            dummy_verts[0].vertex_id = 0; 
            dummy_verts[1].vertex_id = 1;
            dummy_verts[2].vertex_id = 2;
            std::vector<unsigned> indices = { 0, 1, 2 }; 
            ObjectUniforms dummy_ou;
            Material dummy_mat;

            renderer.draw(dummy_verts, indices, dummy_ou, ppu, dummy_mat);

            context.final_color_buffer =
                std::make_shared<std::vector<uint32_t>>(m_framebuffer->m_color_buffer);
        }

        virtual void execute(RenderSystem& renderer, const Scene& scene, const Camera& camera) override {

        }
    private:
        std::shared_ptr<GraphicsPipeline> m_pipeline;
    };
}