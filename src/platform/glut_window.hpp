#pragma once
#include <iostream>
#include <list>
#include <functional>
//#include <GL/freeglut.h>
#include <GL/glut.h>
#include <map>
#include "utils/common.h"
#include "graphics/window.hpp"
// #define WINDOW_USE_CHRONO

#if defined(WINDOW_USE_CHRONO)
    #include <chrono>
#endif

namespace cyx {

// Platform implementation of iterface Window
class GlutWindow  : public Window {

	public:
        GlutWindow(Application* app, const char* title="Glut Window", int width=600, int height=400);
		~GlutWindow();

        auto start() -> void override;
        auto exit() -> void override;

		auto title() -> const char* override;
        auto height() -> u32 override;
        auto width() -> u32 override;

        auto set_cursor(Cursor cur) -> void override;
		auto set_width(u32 new_width) -> void override;

        auto set_mouse(i32 x, i32 y) -> void override;
		auto set_height(u32 new_height) -> void override;

		auto set_fullscreen(bool fullscreen) -> void override;
		auto set_title(const char* title) -> void override;
 	
		auto  delay(u32) -> void override;
		auto  ticks() -> u32 override; 

		auto swap_buffer() -> void override;
		
        auto set_cursor(int type) -> void ;

    public:
		const char* _title;
		u32 _width, _height;

    private:
        auto map_keys() -> void;
        Application* app;

    private:
        static Application* curr_app;
        static GlutWindow* curr_win;
        static Event event;
  	    static std::map<size_t, Key> map;
	};
}
