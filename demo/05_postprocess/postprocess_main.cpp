#include<iostream>
#include<log/log_system.h>
#include<log/macro.h>
#include<application.h>
#include<renderer/scene.h>
#include<resource/assetmanager.h>
#include<global/global_context.h>
#include<renderer/render_system.h>
#include<renderer/pass/main_pass.h>
#include<renderer/pass/shadow_pass.h>
#include<renderer/pass/post_process_pass.h>
using namespace std;
using namespace CGL;
class SpotLightScene :public Scene {
public:
    vec3 redLightPos = vec3(0.f, 1.8f, 1.8f);
    virtual void update() {
        float delta_time = 10;
        CGL::mat4 red_light_model = CGL::rotate(CGL::mat4(1.0f), (float)delta_time * 0.008f, CGL::vec3(0, 1, 0));
        redLightPos = vec3(red_light_model * vec4(redLightPos, 1.0f));
        getLights()[0]->setPosition(redLightPos);
    }
};
std::shared_ptr<SpotLightScene> setScene();

int main() {
    CGL::Application app;
    app.initialize();
    auto renderer = g_runtime_global_context.m_render_system;
    auto m_main_camera = std::make_shared<PerspectiveCamera>(
        vec3(0.9, 0.5, 3.0), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f), 0.1f,
        100.0f, 45.0f, 1.f);

    auto& shadow_vs= std::make_shared<ShadowVertexShader>();
    auto& shadow_fs = std::make_shared<ShadowFragmentShader>();
    auto shadow_pipeline = std::make_shared<GraphicsPipeline>(shadow_vs, shadow_fs);
    auto shadow_pass= std::make_shared<ShadowPass>(4000, shadow_pipeline);

    auto phong_vs = std::make_shared<PCFVertexShader>();
    auto phong_fs = std::make_shared<PCFFragmentShader>();
    auto phong_pipeline = std::make_shared<GraphicsPipeline>(phong_vs, phong_fs);

    auto main_pass = std::make_shared<LightingPass>(phong_pipeline);

    auto post_vs = std::make_shared<PostProcessVertexShader>();
    auto post_fs = std::make_shared<GrayscaleFragmentShader>();
    auto gray_pipeline = std::make_shared<GraphicsPipeline>(post_vs, post_fs);

    auto gray_pass= std::make_shared<PostProcessPass>(gray_pipeline);

    renderer->addPass(shadow_pass);
    renderer->addPass(main_pass);
    renderer->addPass(gray_pass);
    renderer->setScene(setScene());
    renderer->setMainCamera(m_main_camera);
    app.run();
    app.shutdown();
    return 0;
}

std::shared_ptr<SpotLightScene> setScene() {
    std::shared_ptr<SpotLightScene> scene = std::make_shared<SpotLightScene>();
    std::string african_path = "../../../asset/model/african_head/african_head.obj";
    std::string african_texture_path = "../../../asset/model/african_head/african_head_diffuse.png";

    std::string floor_path = "../../../asset/model/floor/floor.obj";
    std::string floor_tex_path = "../../../asset/model/floor/floor_wood.png";
    auto& african_mesh = AssetManager::loadMesh(african_path);
    auto& floor_mesh = AssetManager::loadMesh(floor_path);
    std::shared_ptr<Texture2D> african_texture = std::make_shared<Texture2D>();
    african_texture->load_texture(african_texture_path);
    std::shared_ptr<Texture2D> floor_texture = std::make_shared<Texture2D>();;
    floor_texture->load_texture(floor_tex_path);







    RenderObject obj1(african_mesh);
    //obj1.mesh = african_mesh;
    std::shared_ptr<Material> mat1 = std::make_shared<Material>();
    mat1->m_diffuse_texture = african_texture;
    obj1.material = mat1;


    obj1.transform->set_position(CGL::vec3(0.0f, 0.0f, 0.0f)); // Use Transform
    obj1.transform->set_scale(CGL::vec3(1.0f, 1.0f, 1.0f));    // Use Transform

    //obj1.transform->set_position(CGL::vec3(0.0f, -0.2f, 0.0f)); // Use Transform
    //obj1.transform->set_scale(CGL::vec3(0.7, 0.7, 0.7));    // Use Transform



    RenderObject obj2(floor_mesh);
    obj2.mesh = floor_mesh;
    std::shared_ptr<Material> mat2 = std::make_shared<Material>();
    mat2->m_diffuse_texture = floor_texture;
    obj2.material = mat2;
    obj2.transform->set_position(CGL::vec3(0, 0, 0)); // Use Transform
    obj2.transform->set_scale(CGL::vec3(1.2f, 1.0f, 1.2f));    // Use Transform
    scene->addObject(obj1);
    scene->addObject(obj2);

    std::string cube_filename = "../../../asset/model/cube/cube.obj";
    auto& cube_mesh = AssetManager::loadMesh(cube_filename);

    RenderObject red_obj(cube_mesh);
    std::shared_ptr<Material> red_mat = std::make_shared<Material>();
    red_mat->diffuse_color = vec3(1.0f, 1.0f, 1.0f);
    red_obj.material = red_mat;
    red_obj.transform->set_position(vec3(0, -0.05, 1.2));
    red_obj.transform->set_scale(CGL::vec3(1, 1, 1));
   // scene->addObject(red_obj);
    auto red_point_light = std::make_shared<PointLight>(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 1.8f, 1.8f),
        vec3(0.002f, 0.01f, 0.03f)); // Increased intensity
    scene->addLight(red_point_light);
    return scene;
}
