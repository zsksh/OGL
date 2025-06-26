#include<global/global_context.h>
#include<log/log_system.h>
#include<renderer/render_system.h>
#include<platform/window_system.h>
#include<platform/window_system.h>
//#include<input/input_system.h>
namespace CGL {
	RuntimeGlobalContext g_runtime_global_context;
	// create all global systems and initialize these systems
	void RuntimeGlobalContext::startSystems(const std::string& config_file_path) {
		m_logger_system = std::make_shared<LogSystem>();
		m_window_system = std::make_shared<WindowSystem>();
		//m_asset_manager = std::make_shared<AssetManager>();
		WindowCreateInfo window_create_info;
		m_window_system->initialize(window_create_info);

		//m_input_system = std::make_shared<InputSystem>();
		//m_input_system->initialize();

		m_render_system = std::make_shared<RenderSystem>();
		m_render_system->initialize(window_create_info);
	}
	// destroy all global systems
	void RuntimeGlobalContext::shutdownSystems() {


		m_logger_system.reset(); // why need this line?
	}


}