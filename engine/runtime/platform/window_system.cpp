#include<platform/window_system.h>
#include<global/global_context.h>
#include<log/macro.h>
#include<renderer/render_system.h>
namespace CGL {
	void WindowSystem::initialize(WindowCreateInfo create_info) {
		m_width = create_info.width;
		m_height = create_info.height;

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			throw std::runtime_error("SDL initialization failed");
		}
		m_window = SDL_CreateWindow("Software Renderer", m_width, m_height, 0);
		if (!m_window) {
			SDL_Quit();
			throw std::runtime_error("Window creation failed");
		}
		m_renderer = SDL_CreateRenderer(m_window, NULL);
		if (!m_renderer) {
			SDL_DestroyWindow(m_window);
			SDL_Quit();
			throw std::runtime_error("Renderer creation failed");
		}
		// Create a texture that matches the framebuffer size
		m_framebuffer_texture = SDL_CreateTexture(
			m_renderer,
			SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_STREAMING,
			m_width,
			m_height
		);
		if (!m_framebuffer_texture) {
			throw std::runtime_error(std::string("Texture creation failed: ") + SDL_GetError());
		}
	}

	WindowSystem::~WindowSystem() {

	}

	void WindowSystem::poll_events() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				m_should_close = true;
				break;

				// Window gained keyboard focus
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				//std::cout << " gained focus." << std::endl;
				// You could add visual feedback, like changing a border color
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				///  getchar();
				break;
			default:


				break;
			}
		}

	}
	void WindowSystem::render_frame() {
		update_fps_counter();
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);
		update_framebuffer();
		if (!m_fps_debug_text.empty()) {
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255); // Green text
			SDL_RenderDebugText(m_renderer, 10.0f, 10.0f, m_fps_debug_text.c_str());
		}
		SDL_RenderPresent(m_renderer);
	}
	void WindowSystem::update_framebuffer() {
		auto& render_system = g_runtime_global_context.m_render_system;
		auto& frame_buffer = render_system->getFramebuffer()->m_color_buffer;
		std::vector<unsigned> colors;
		//frame_buffer->resolve(colors);
		// Lock texture for writing
		void* pixels;
		int pitch;
		if (!SDL_LockTexture(m_framebuffer_texture, nullptr, &pixels, &pitch)) {
			LOG_ERROR("SDL_LockTexture failed: {0}", SDL_GetError());
			//std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
			return; // Error locking texture
		}

		// Copy framebuffer to texture
		uint32_t* tex_pixels = static_cast<uint32_t*>(pixels);
		for (int y = 0; y < m_height; y++) {
			int flipped_y = m_height - 1 - y;
			for (int x = 0; x < m_width; x++) {
				int fb_index = (flipped_y * m_width + x) * 4; // Assuming RGBA format
				uint32_t pixel =
					(frame_buffer[fb_index + 0] << 0) |  // R
					(frame_buffer[fb_index + 1] << 8) |  // G
					(frame_buffer[fb_index + 2] << 16) |  // B
					(0xFF << 24);
				tex_pixels[y * (pitch / 4) + x] = pixel;
			}
		}

		SDL_UnlockTexture(m_framebuffer_texture);

		// Render the texture
		SDL_RenderTexture(m_renderer, m_framebuffer_texture, nullptr, nullptr);
	}
	void WindowSystem::update_fps_counter() {
		const auto now = clock::now();
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_fps_update).count();

		m_frame_count++;

		// Update every second
		if (elapsed > 1000) {
			m_current_fps = m_frame_count * 1000.0f / elapsed;
			m_average_fps = m_average_fps * (1.0f - m_fps_alpha) + m_current_fps * m_fps_alpha;

			// Update window title with FPS info
			std::ostringstream title;
			title << "Software Renderer | FPS: " << std::fixed << std::setprecision(1) << m_current_fps
				<< " | Avg: " << m_average_fps;
			m_fps_debug_text = "FPS: " + std::to_string(static_cast<int>(m_current_fps)) +
				" (Avg: " + std::to_string(static_cast<int>(m_average_fps)) + ")";

			SDL_SetWindowTitle(m_window, title.str().c_str());
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
			// Reset counters
			m_frame_count = 0;
			m_last_fps_update = now;
		}

		//**SDL_RenderDebugText(m_renderer, 100, 100, title.str().c_str());
		//SDL_RenderPresent(m_renderer); // <-- Required to display changes
	}

}