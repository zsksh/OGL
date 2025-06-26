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
    vec3 redLightPos = vec3(0, -0.05, 1.2);
    vec3 greenLightPos = vec3(0.87, -0.05, -0.87);
    vec3 blueLightPos = vec3(-0.83, -0.05, -0.83);

    virtual void update() {
        float delta_time = 10;
        CGL::mat4 red_light_model = CGL::rotate(CGL::mat4(1.0f), (float)delta_time * 0.008f, CGL::vec3(1, 1, 0));
        auto& red_light_obj = getObjects()[2];
        redLightPos = vec3(red_light_model * vec4(redLightPos, 1.0f));
        red_light_obj.transform->set_position(redLightPos);
        getLights()[0]->setPosition(redLightPos);

        CGL::mat4 green_light_model = CGL::rotate(CGL::mat4(1.0f), (float)delta_time * 0.008f, CGL::vec3(1, 1, 1));
        auto& green_light_obj = getObjects()[3];
        greenLightPos = vec3(green_light_model * vec4(greenLightPos, 1.0f));
        green_light_obj.transform->set_position(greenLightPos);
        getLights()[1]->setPosition(greenLightPos);

        CGL::mat4 blue_light_model = CGL::rotate(CGL::mat4(1.0f), (float)delta_time * 0.008f, CGL::vec3(-1, 1, 1));
        auto& blue_light_obj = getObjects()[4];
        blueLightPos = vec3(blue_light_model * vec4(blueLightPos, 1.0f));
        blue_light_obj.transform->set_position(blueLightPos);
        getLights()[2]->setPosition(blueLightPos);
        std::cout << "update scene" << std::endl;
    }
};
std::shared_ptr<LightScene> createScence();
int main() {
    CGL::Application app;
    app.initialize();
    auto renderer = g_runtime_global_context.m_render_system;
    auto phong_vs = std::make_shared<PhongVertexShader>();
    auto phong_fs = std::make_shared<PhongFragmentShader>();
    auto phong_pipeline = std::make_shared<GraphicsPipeline>(phong_vs, phong_fs);

    auto main_pass = std::make_shared<LightingPass>(phong_pipeline);
    renderer->addPass(main_pass);


    auto m_main_camera = std::make_shared<PerspectiveCamera>(
        vec3(0.9, 0.5, 3.0), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f), 0.1f,
        100.0f, radians(45.0f), 1.f);
    renderer->setScene(createScence());
    renderer->setMainCamera(m_main_camera);
    app.run();
    app.shutdown();
    return 0;
}

std::shared_ptr<LightScene> createScence() {
    std::shared_ptr<LightScene> scene = std::make_shared<LightScene>();

    std::string char_path = "../../../asset/model/mary/Marry.obj";
    std::string char_tex_path = "../../../asset/model/mary/MC003_Kozakura_Mari.png";
    std::string floor_path = "../../../asset/model/floor/floor.obj";
    std::string floor_tex_path = "../../../asset/model/floor/floor_wood.png";

    RenderObject character = createRenderObject(char_path, char_tex_path, vec3(0.0f, -1.0f, 0.0f), vec3(0.5, 0.5,0.5));
    RenderObject floor = createRenderObject(floor_path, floor_tex_path, vec3(0, 0, 0), vec3(1.2, 1.0, 1.2));
    scene->addObject(character);
    scene->addObject(floor);

// lights
    std::string cube_filename = "../../../asset/model/cube/cube.obj";
    RenderObject red_light_obj = createRenderObject(cube_filename, vec3(1.0f, 0.0f, 0.0f),
        vec3(0, -0.05, 1.2), vec3(1, 1, 1));

    RenderObject green_light_obj = createRenderObject(cube_filename, vec3(0.0f, 1.0f, 0.0f),
        vec3(0.87, -0.05, -0.87), vec3(1, 1, 1));
    RenderObject blue_light_obj = createRenderObject(cube_filename, vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.83, -0.05, -0.83), vec3(1, 1, 1));
   
    scene->addObject(red_light_obj);
    scene->addObject(green_light_obj);
    scene->addObject(blue_light_obj);

    auto red_point_light = std::make_shared<PointLight>(vec3(0.1f, 0.0f, 0.0f), vec3(0, -0.05, 1.2),
        vec3(0.1f, 0.1f, 0.1f)); 

    auto green_point_light = std::make_shared<PointLight>(vec3(0.0f, 0.1f, 0.0f), vec3(0.87, -0.05, -0.87),
        vec3(0.1f, 0.1f, 0.1f)); 

    auto blue_point_light = std::make_shared<PointLight>(vec3(0.0f, 0.0f, 0.1f), vec3(-0.83, -0.05, -0.83),
        vec3(0.1f, 0.1f, 0.1f)); 

    scene->addLight(red_point_light);
    scene->addLight(green_point_light);
    scene->addLight(blue_point_light);

    return scene;
}













