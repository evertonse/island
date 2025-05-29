#pragma once
#include <iostream>
#include <list>
#include <functional>
#include <map>

#include "GLFW/glfw3.h"
#include "utils/common.h"
#include "graphics/window.hpp"
// #define WINDOW_USE_CHRONO


#include <GLFW/glfw3.h>

#if defined(_WIN32) || defined(__MINGW32__)
    #include <windows.h> // Include Windows.h for Sleep
#else
#if defined(WINDOW_USE_CHRONO)
    #include <thread>
    #include <chrono>
#endif
#endif

namespace cyx {

// Platform implementation of interface Window using GLFW
class GlfwWindow : public Window {
public:
    GlfwWindow(Application* app, const char* title = "Glfw Window", int width = 600, int height = 400);
    ~GlfwWindow();

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
    
    auto delay(u32) -> void override;
    auto ticks() -> u32 override;

    auto swap_buffer() -> void override;
    
    // Additional GLFW-specific functionality if needed
    auto native_window() -> GLFWwindow* { return _window; }

private:
    auto map_keys() -> void;
    auto setup_callbacks() -> void;

    Application* app = nullptr;
    GLFWwindow* _window = nullptr;
    const char* _title = nullptr;
    u32 _width = 0, _height = 0;
    i32 _restore_width = 0, _restore_height = 0;
    i32 _restore_x = 0, _restore_y = 0;

private:
    static Application* curr_app;
    static GlfwWindow* curr_win;
    static Event event;
    static std::map<size_t, Key> map;
};

} // namespace cyx
