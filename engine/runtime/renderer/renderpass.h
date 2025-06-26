#pragma once
#include<string>
#include<vector>
#include<renderer/scene.h>
#include<renderer/camera.h>
#include<renderer/framebuffer.h>
namespace CGL {
    class RenderSystem;
    class RenderPass {
    public:
        RenderPass(std::string name) : m_name(std::move(name)) {}
        virtual ~RenderPass() = default;

        virtual void execute(RenderSystem& renderer, const Scene& scene, const Camera& camera) = 0;

        virtual void execute(RenderSystem& renderer, RenderContext& context) = 0;
        const std::string& getName() const { return m_name; }
        virtual const Color* getOutputBuffer() const { return nullptr; }
        std::shared_ptr<Framebuffer>& getFrameBuffer() { return m_framebuffer; }
    protected:
        std::shared_ptr<Framebuffer> m_framebuffer = nullptr;;
        std::string m_name;
    };



}