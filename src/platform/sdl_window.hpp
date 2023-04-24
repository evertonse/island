#pragma once
#include <iostream>
#include <list>
#include <functional>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>
#include "utils/common.h"
#include "graphics/window.hpp"

namespace cyx {

	class SDLWindow : public Window {
	
	public:
		using Event = SDL_Event;
		using Key   = u8; 
		using EventCallback = void (*)(Event&);
		
		SDLWindow(const char* title, int width = 640, int height = 640);
		~SDLWindow();

		auto on_update() -> void;
		auto on_event(SDL_Event& e)  -> void;

		auto clear() -> void;

		auto width()->u32;
		auto height()->u32;
		auto keys() -> const u8*;
		auto add_callback(EventCallback fn) -> void;
		auto remove_callback(EventCallback fn) -> void;

		auto set_width(u32 new_width) -> void;
		auto set_height(u32 new_height) -> void;

		auto set_fullscreen(bool fullscreen) -> void;
		auto set_title(const char* title) -> void;
		
		auto	delay(u32) -> void;

		auto	ticks() -> u32;
		auto	title() -> const char*;
		

		auto swap_buffer() -> void;

		auto window_handle () -> SDL_Window*;
		auto context_handle() -> SDL_GLContext*;
		
		auto poll_events() -> void;
		
		auto native() -> SDL_Window*;

	private:
		SDL_Window* _window;
		SDL_GLContext _glcontext;
		const char* _title;
		u32 _width, _height;

		std::list<EventCallback> _event_callbacks;
		const u8* _keys;
	};
}
