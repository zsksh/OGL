#pragma once
#include <chrono>  // For high-resolution timing
#include <string>  // For string manipulation
#include <sstream> // For string stream
#include <iomanip> // For setprecision
#include<SDL3/SDL.h>
namespace CGL {
	struct WindowCreateInfo
	{
		int         width{ 1000 };
		int         height{ 1000 };
		const char* title{ "ByteTroop" };
		bool        is_fullscreen{ false };
	};
	class WindowSystem final {
	public:
		WindowSystem() = default;
		~WindowSystem();
		void               initialize(WindowCreateInfo create_info);
		void render_frame();
		void poll_events();
		void update_framebuffer();
		void update_fps_counter();


		bool should_close() { return m_should_close; }
	private:
		// FPS calculation variables
		using clock = std::chrono::high_resolution_clock;
		std::chrono::time_point<clock> m_last_fps_update;
		int m_frame_count = 0;
		float m_current_fps = 0.0f;
		float m_average_fps = 0.0f;
		float m_fps_alpha = 0.1f; // Smoothing factor for average FPS
	private:
		int m_width{ 0 };
		int m_height{ 0 };
		bool m_should_close = false;
	private: // SDL3;
		SDL_Renderer* m_renderer = nullptr;
		SDL_Window* m_window = nullptr;
		SDL_Texture* m_framebuffer_texture = nullptr;
		std::string m_fps_debug_text; // Stores the current FPS string for rendering

	};




}