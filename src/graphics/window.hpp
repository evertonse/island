#pragma once
#include "utils/common.h"
#include "graphics/application.hpp"
#include "graphics/event.hpp"

namespace cyx {

    enum Cursor {
        NONE,ARROW
    };

    struct Window {
	
        virtual auto start() -> void = 0;
        virtual auto exit() -> void = 0;

		virtual auto title() -> const char* = 0;
		virtual auto width()-> u32 = 0;
		virtual auto height()-> u32 = 0;


		virtual auto set_width(u32 new_width) -> void = 0;
		virtual auto set_height(u32 new_height) -> void = 0;

        virtual auto set_mouse(Cursor cur) -> void = 0;
		virtual auto set_mouse(i32 x, i32 y) -> void = 0;
		virtual auto set_fullscreen(bool fullscreen) -> void = 0;
		virtual auto set_title(const char* title) -> void = 0;
		
		virtual auto  delay(u32) -> void = 0;
		virtual auto  ticks() -> u32 = 0;

		virtual auto swap_buffer() -> void = 0;
        friend class Application;
	};
}
