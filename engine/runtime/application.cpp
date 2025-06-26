#include<global/global_context.h>
#include<application.h>
#include<platform/window_system.h>
#include<renderer/render_system.h>
#include<log/macro.h>
namespace CGL {
	void Application::initialize() {
		g_runtime_global_context.startSystems(std::string(""));

	}
	void Application::run() {
		auto window_system = g_runtime_global_context.m_window_system;
		while (!window_system->should_close()) {
			tick_one_frame();
		}
	}
	void Application::tick_one_frame() {
		//animation();
		g_runtime_global_context.m_render_system->tick();
		g_runtime_global_context.m_window_system->poll_events();
		g_runtime_global_context.m_window_system->render_frame();
		//g_runtime_global_context.m_window_system->swap_buffers();
	}
	void Application::shutdown() {
		LOG_INFO("shutdown");
		g_runtime_global_context.shutdownSystems();

		//Reflection::TypeMetaRegister::metaUnregister();
	}
}