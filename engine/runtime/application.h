#pragma once 
#include<functional>
namespace CGL {
	class Application {
	public:
		void initialize();
		void run();
		void tick_one_frame();
		void shutdown();


		void set_animation(std::function<void()> func) { animation_callback = std::move(func); }
		void animation() {
			if (animation_callback != nullptr)
				animation_callback();
		}
	private:
		std::function<void()> animation_callback = nullptr;;
	};
}