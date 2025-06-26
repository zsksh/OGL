#include<iostream>
#include<log/log_system.h>
#include<log/macro.h>
#include<application.h>
#include<renderer/scene.h>
#include<resource/assetmanager.h>
#include<global/global_context.h>
#include<renderer/render_system.h>
#include<renderer/pass/main_pass.h>

using namespace std;
using namespace CGL;
class LightScene :public Scene {
public:
    vec3 light_pos = vec3(0.1, 1.3, 0);

    virtual void update() {
        float delta_time = 10;
        CGL::mat4 light_model = CGL::rotate(CGL::mat4(1.0f), (float)delta_time * 0.008f, CGL::vec3(0, 1, 0));
        auto ight_obj = getObjects()[2];
        light_pos = vec3(light_model * vec4(light_pos, 1.0f));
        ight_obj.transform->set_position(light_pos);
        getLights()[0]->setPosition(light_pos);
    }
};
std::shared_ptr<LightScene> setScene();

int main() {
    CGL::Application app;
    app.initialize();
    auto renderer = g_runtime_global_context.m_render_system;
    auto m_main_camera = std::make_shared<PerspectiveCamera>(
        vec3(1.7, 0.5, 3.6), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f), 0.1f,
        100.0f, 45.0f, 1.f);
    auto phong_vs = std::make_shared<PhongVertexShader>();
    auto phong_fs = std::make_shared<PhongFragmentShader>();
    auto phong_pipeline = std::make_shared<GraphicsPipeline>(phong_vs, phong_fs);

    auto main_pass = std::make_shared<LightingPass>(phong_pipeline);
    renderer->addPass(main_pass);
    renderer->setScene(setScene());
    renderer->setMainCamera(m_main_camera);
    app.run();
    app.shutdown();
    return 0;


    renderer->setScene(setScene());
    renderer->setMainCamera(m_main_camera);
    app.run();
    app.shutdown();
    return 0;
}

std::shared_ptr<LightScene> setScene() {
    std::shared_ptr<LightScene> scene = std::make_shared<LightScene>();
    std::string char_path = "../../../asset/model/spot/spot.obj";
    std::string char_tex_path = "../../../asset/model/spot/spot_texture.png";
    std::string floor_path = "../../../asset/model/floor/floor.obj";
    std::string floor_tex_path = "../../../asset/model/floor/floor_wood.png";

    RenderObject character = createRenderObject(char_path, char_tex_path, vec3(0.0f, -0.5f, 0.0f), vec3(0.6, 0.6, 0.6),
        CGL::vec3(0, radians(190), 0));
    RenderObject floor = createRenderObject(floor_path, floor_tex_path, vec3(0, 0, 0), vec3(1.2, 1.0, 1.2));
    scene->addObject(character);
    scene->addObject(floor);


    std::string cube_filename = "../../../asset/model/cube/cube.obj";
    RenderObject lightobj = createRenderObject(cube_filename, vec3(1.0f, 1.0f, 1.0f),
        vec3(0, -0.05, 1.2),vec3(1, 1, 1));
    scene->addObject(lightobj);

    auto spot_light = std::make_shared<SpotLight>(vec3(0.2, 0.2, 0.2), vec3(0.4, 1.5, 0),
        vec3(0.1f, 0.1f, 0.1f),vec3(0,-1,0),cos(radians(17.0f)), cos(radians(20.0f)));
    scene->addLight(spot_light);
    return scene;
}













/*#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>

// --- 1. Basic Graphics Primitives & Data Structs ---
//    (Simplified placeholders for vector math, etc.)

struct vec3 { float x = 0, y = 0, z = 0; };
struct mat4 {
    // Fake matrix to show it's being passed around
    std::string name = "identity";
    vec3 operator*(const vec3& v) const { return v; } // Fake transform
};

struct Color { float r = 0, g = 0, b = 0, a = 0; };
struct VertexData { vec3 pos; vec3 normal; };
struct FragmentData { vec3 world_pos; };

// --- Uniforms: The "Descriptor Set" / "Uniform Buffer" concept ---
struct SceneUniforms {
    mat4 view;
    mat4 projection;
    vec3 light_pos;
};

struct ObjectUniforms {
    mat4 model;
};

// --- Resource Wrappers ---
using VertexBuffer = std::vector<VertexData>;
struct Framebuffer {
    std::string name;
    int width, height;
};

// --- 2. Shader Interfaces and Concrete Implementations ---

class VertexShader {
public:
    virtual ~VertexShader() = default;
    virtual void shade(VertexData& v_out, const VertexData& v_in, const ObjectUniforms& obj, const SceneUniforms& scene) = 0;
};

class FragmentShader {
public:
    virtual ~FragmentShader() = default;
    virtual Color shade(const FragmentData& frag, const SceneUniforms& scene) = 0;
};

class SimpleVertexShader : public VertexShader {
public:
    void shade(VertexData& v_out, const VertexData& v_in, const ObjectUniforms& obj, const SceneUniforms& scene) override {
        // In a real shader, this would be matrix multiplication.
        // v_out.pos = scene.projection * scene.view * obj.model * v_in.pos;
        v_out = v_in; // Simplified for this example
        std::cout << "    VS: Shading vertex at (" << v_in.pos.x << ", " << v_in.pos.y << "). Model: " << obj.model.name << std::endl;
    }
};

class SimpleFragmentShader : public FragmentShader {
public:
    Color shade(const FragmentData& frag, const SceneUniforms& scene) override {
        // In a real shader, this would be lighting calculations.
        std::cout << "        FS: Shading fragment at (" << frag.world_pos.x << ", " << frag.world_pos.y << ") with light at (" << scene.light_pos.x << ")" << std::endl;
        return { 1.0f, 0.5f, 0.2f, 1.0f }; // Return a fixed color
    }
};


// --- 3. The "Vulkan-like" API Classes ---

class GraphicsPipeline {
public:
    GraphicsPipeline(std::shared_ptr<VertexShader> vs, std::shared_ptr<FragmentShader> fs)
        : m_vertex_shader(std::move(vs)), m_fragment_shader(std::move(fs)) {
        std::cout << "[Setup] GraphicsPipeline created." << std::endl;
    }

    VertexShader* get_vertex_shader() const { return m_vertex_shader.get(); }
    FragmentShader* get_fragment_shader() const { return m_fragment_shader.get(); }

private:
    std::shared_ptr<VertexShader> m_vertex_shader;
    std::shared_ptr<FragmentShader> m_fragment_shader;
};


// Forward declaration for CommandBuffer
struct RendererState;
// This internal struct holds the currently bound state during command execution
struct RendererState {
    GraphicsPipeline* active_pipeline = nullptr;
    VertexBuffer* active_vertex_buffer = nullptr;
    Framebuffer* active_framebuffer = nullptr;
    SceneUniforms active_scene_uniforms;
    ObjectUniforms active_object_uniforms;
};

class CommandBuffer {
public:
    void clear() { m_commands.clear(); }

    void cmd_begin_render_pass(Framebuffer* fb) {
        m_commands.emplace_back([=](RendererState& state) {
            std::cout << "\n[Exec] Beginning Render Pass on Framebuffer '" << fb->name << "'" << std::endl;
            state.active_framebuffer = fb;
        });
    }
    void cmd_end_render_pass() {
        m_commands.emplace_back([=](RendererState& state) {
            state = {};
        });
    }


    void cmd_bind_pipeline(GraphicsPipeline* pipeline) {
        m_commands.emplace_back([=](RendererState& state) {
            std::cout << "[Exec] Binding Graphics Pipeline" << std::endl;
            state.active_pipeline = pipeline;
        });
    }

    void cmd_bind_vertex_buffer(VertexBuffer* vb) {
        m_commands.emplace_back([=](RendererState& state) {
            state.active_vertex_buffer = vb;
        });
    }

    // Using copies for uniforms is safer for command buffers
    void cmd_bind_scene_uniforms(SceneUniforms uniforms) {
        m_commands.emplace_back([=](RendererState& state) {
            state.active_scene_uniforms = uniforms;
        });
    }

    void cmd_bind_object_uniforms(ObjectUniforms uniforms) {
        m_commands.emplace_back([=](RendererState& state) {
            state.active_object_uniforms = uniforms;
        });
    }

    void cmd_draw(size_t vertex_count) {
        m_commands.emplace_back([=](RendererState& state) {
            std::cout << "[Exec] Executing Draw Call for " << vertex_count << " vertices" << std::endl;

            // --- This is where the core rendering loop is triggered ---
            if (!state.active_pipeline || !state.active_vertex_buffer) {
                std::cout << "    [Error] Missing pipeline or vertex buffer!" << std::endl;
                return;
            }

            auto* vs = state.active_pipeline->get_vertex_shader();
            auto* fs = state.active_pipeline->get_fragment_shader();

            for (size_t i = 0; i < vertex_count; ++i) {
                const VertexData& v_in = (*state.active_vertex_buffer)[i];
                VertexData v_out;

                // 1. Vertex Shading
                vs->shade(v_out, v_in, state.active_object_uniforms, state.active_scene_uniforms);

                // 2. Rasterization (SIMPLIFIED: one fragment per vertex)
                // In a real renderer, you'd process 3 vertices at a time to form a triangle
                // and interpolate attributes to generate many fragments.
                FragmentData frag_in;
                frag_in.world_pos = v_out.pos;

                // 3. Fragment Shading
                Color final_color = fs->shade(frag_in, state.active_scene_uniforms);
                // 4. Per-Fragment Operations (Depth test, blending, write to framebuffer)
                // (Skipped for this example)
            }
        });
    }

private:
    friend class Renderer;
    std::vector<std::function<void(RendererState&)>> m_commands;
};



class Renderer {
public:
    CommandBuffer begin_command_buffer() {
        return CommandBuffer(); // Return a new, empty command buffer
    }

    void submit(CommandBuffer& cmd_buffer) {
        std::cout << "\n--- RENDERER SUBMIT ---" << std::endl;
        RendererState execution_state;
        for (const auto& command : cmd_buffer.m_commands) {
            command(execution_state);
        }
        std::cout << "--- SUBMIT COMPLETE ---\n" << std::endl;
    }
};


// --- 4. The Main Application ---

int main() {
    std::cout << "--- APPLICATION START ---\n" << std::endl;

    // --- SETUP PHASE (Happens once at load time) ---
    Renderer renderer;

    // Create Shaders
    auto vs = std::make_shared<SimpleVertexShader>();
    auto fs = std::make_shared<SimpleFragmentShader>();

    // Create a Pipeline State Object (PSO)
    auto pipeline = std::make_shared<GraphicsPipeline>(vs, fs);

    // Create vertex data for two different models
    VertexBuffer cube_vertices = { {{-1,-1,0}}, {{1,-1,0}}, {{0,1,0}} };
    VertexBuffer sphere_vertices = { {{-2, -2, -5}}, {{2, -2, -5}}, {{0, 2, -5}} };

    // Create a render target
    Framebuffer main_framebuffer = { "Main Window", 800, 600 };

    // Prepare uniform data structs
    SceneUniforms scene_uniforms;
    scene_uniforms.view.name = "CameraViewMatrix";
    scene_uniforms.projection.name = "PerspectiveMatrix";

    ObjectUniforms cube_uniforms;
    cube_uniforms.model.name = "CubeModelMatrix";

    ObjectUniforms sphere_uniforms;
    sphere_uniforms.model.name = "SphereModelMatrix";

    // --- RENDER LOOP (Happens every frame) ---
    for (int frame = 0; frame < 2; ++frame) {
        std::cout << "==================== FRAME " << frame << " ====================" << std::endl;

        // Update dynamic data (e.g., light moves every frame)
        scene_uniforms.light_pos = { 10.0f + frame * 5.0f, 20.0f, 0.0f };

        // 1. Get a fresh command buffer
        CommandBuffer cmd = renderer.begin_command_buffer();

        // 2. Record commands for this frame
        cmd.cmd_begin_render_pass(&main_framebuffer);
        cmd.cmd_bind_pipeline(pipeline.get());
        cmd.cmd_bind_scene_uniforms(scene_uniforms); // Set scene-wide data once

        // Draw the cube
        cmd.cmd_bind_object_uniforms(cube_uniforms); // Bind cube's data
        cmd.cmd_bind_vertex_buffer(&cube_vertices);
        cmd.cmd_draw(cube_vertices.size());

        // Draw the sphere
        cmd.cmd_bind_object_uniforms(sphere_uniforms); // Bind sphere's data
        cmd.cmd_bind_vertex_buffer(&sphere_vertices);
        cmd.cmd_draw(sphere_vertices.size());

        cmd.cmd_end_render_pass();

        // 3. Submit the completed command buffer for execution
        renderer.submit(cmd);
    }

    std::cout << "--- APPLICATION END ---" << std::endl;
    return 0;
}*/