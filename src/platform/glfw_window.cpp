#include "platform/glfw_window.hpp"
#include "glfw_window.hpp"


namespace cyx {

Application *GlfwWindow::curr_app = nullptr;
GlfwWindow *GlfwWindow::curr_win = nullptr;
Event GlfwWindow::event = Event{};
std::map<size_t, Key> GlfwWindow::map = std::map<size_t, Key>();

GlfwWindow::GlfwWindow(Application *app, const char *title, int width, int height) {
   // Initialize GLFW

   this->app = app;
   GlfwWindow::curr_app = app;
   GlfwWindow::curr_win = this;

   if (!glfwInit()) {
      std::cout << '[' << __func__ << ']' << "Failed to initialize glfw" << std::endl;
      std::cout.flush();
      std::exit(69);
      return;
   }

   this->app->on_init(*this); // OBS: can't call gl functions from here

   // Set window hints
   glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing (equivalent to GLUT_MULTISAMPLE)
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   this->_width = width;
   this->_height = height;

   // Create _window
   _window = glfwCreateWindow(width, height, title, nullptr, nullptr);

   if (!_window) {
      glfwTerminate();
      std::cout << '[' << __func__ << ']' << "Failed to create glfw window" << std::endl;
      std::cout.flush();
      std::exit(69);
      return;
   }

   // Center window on screen
   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   glfwSetWindowPos(_window, (mode->width - width) / 2, (mode->height - height) / 2);

   glfwMakeContextCurrent(_window);
   this->app->on_create(*this);

   map_keys();
   setup_callbacks();
}

GlfwWindow::~GlfwWindow() {
   if (_window) {
      glfwDestroyWindow(_window);
   }
   glfwTerminate();
}

void GlfwWindow::setup_callbacks() {
   // Set user pointer for callbacks
   glfwSetWindowUserPointer(_window, this);

   // Key callback
   glfwSetKeyCallback(_window, [](GLFWwindow *_window, int key, int scancode, int action, int mods) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      if (action == GLFW_PRESS) {
         e.type = EventType::KEYDOWN;
      } else if (action == GLFW_RELEASE) {
         e.type = EventType::KEYUP;
      } else {
         return; // Ignore GLFW_REPEAT
      }

      // Handle modifier keys
      if (mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
         e.key = Key::SHIFT;
         app->on_event(*win, e);
         return;
      } else if (mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
         e.key = Key::CTRL;
         app->on_event(*win, e);
         return;
      }

      auto it = map.find(key);
      if (it != map.end()) {
         e.key = it->second;
         app->on_event(*win, e);
      }
   });

   // Character callback for text input
   glfwSetCharCallback(_window, [](GLFWwindow *_window, unsigned int codepoint) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      e.type = EventType::KEYDOWN;
      if (codepoint >= 'a' && codepoint <= 'z') {
         codepoint = codepoint - 'a' + 'A';
      }

      auto it = map.find(codepoint);
      if (it != map.end()) {
         e.key = it->second;
         app->on_event(*win, e);
      }
   });

   // Mouse button callback
   glfwSetMouseButtonCallback(_window, [](GLFWwindow *_window, int button, int action, int mods) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      if (action == GLFW_PRESS) {
         e.type = EventType::MOUSEDOWN;
      } else if (action == GLFW_RELEASE) {
         e.type = EventType::MOUSEUP;
      }

      switch (button) {
      case GLFW_MOUSE_BUTTON_LEFT:
         e.mouse.button = MouseButton::LEFT;
         break;
      case GLFW_MOUSE_BUTTON_MIDDLE:
         e.mouse.button = MouseButton::MIDDLE;
         break;
      case GLFW_MOUSE_BUTTON_RIGHT:
         e.mouse.button = MouseButton::RIGHT;
         break;
      }

      app->on_event(*win, e);
   });

   // Mouse move callback
   glfwSetCursorPosCallback(_window, [](GLFWwindow *_window, double xpos, double ypos) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      e.type = EventType::MOUSEMOVE;
      e.mouse.x = static_cast<int>(xpos);
      e.mouse.y = static_cast<int>(ypos);
      app->on_event(*win, e);
   });

   // Mouse scroll callback
   glfwSetScrollCallback(_window, [](GLFWwindow *_window, double xoffset, double yoffset) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      e.type = EventType::MOUSEWHEEL;
      e.mouse.wheel.dir = (yoffset > 0) ? 1 : -1;
      app->on_event(*win, e);
   });

   // Window resize callback
   glfwSetFramebufferSizeCallback(_window, [](GLFWwindow *_window, int width, int height) {
      auto *win = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(_window));
      auto &app = GlfwWindow::curr_app;
      Event &e = GlfwWindow::event;

      win->_width = width;
      win->_height = height;
      e.type = EventType::RESHAPE;
      app->on_event(*win, e);
   });
}

auto GlfwWindow::map_keys() -> void {
   // Letters
   map[GLFW_KEY_A] = Key::A;
   map[GLFW_KEY_B] = Key::B;
   map[GLFW_KEY_C] = Key::C;
   map[GLFW_KEY_D] = Key::D;
   map[GLFW_KEY_E] = Key::E;
   map[GLFW_KEY_F] = Key::F;
   map[GLFW_KEY_G] = Key::G;
   map[GLFW_KEY_H] = Key::H;
   map[GLFW_KEY_I] = Key::I;
   map[GLFW_KEY_J] = Key::J;
   map[GLFW_KEY_K] = Key::K;
   map[GLFW_KEY_L] = Key::L;
   map[GLFW_KEY_M] = Key::M;
   map[GLFW_KEY_N] = Key::N;
   map[GLFW_KEY_O] = Key::O;
   map[GLFW_KEY_P] = Key::P;
   map[GLFW_KEY_Q] = Key::Q;
   map[GLFW_KEY_R] = Key::R;
   map[GLFW_KEY_S] = Key::S;
   map[GLFW_KEY_T] = Key::T;
   map[GLFW_KEY_U] = Key::U;
   map[GLFW_KEY_V] = Key::V;
   map[GLFW_KEY_W] = Key::W;
   map[GLFW_KEY_X] = Key::X;
   map[GLFW_KEY_Y] = Key::Y;
   map[GLFW_KEY_Z] = Key::Z;

   // Function keys
   map[GLFW_KEY_F1] = Key::F1;
   map[GLFW_KEY_F2] = Key::F2;
   map[GLFW_KEY_F3] = Key::F3;
   map[GLFW_KEY_F4] = Key::F4;
   map[GLFW_KEY_F5] = Key::F5;
   map[GLFW_KEY_F6] = Key::F6;
   map[GLFW_KEY_F7] = Key::F7;
   map[GLFW_KEY_F8] = Key::F8;
   map[GLFW_KEY_F9] = Key::F9;
   map[GLFW_KEY_F10] = Key::F10;
   map[GLFW_KEY_F11] = Key::F11;
   map[GLFW_KEY_F12] = Key::F12;

   // Arrow keys
   map[GLFW_KEY_DOWN] = Key::ARROWDOWN;
   map[GLFW_KEY_LEFT] = Key::ARROWLEFT;
   map[GLFW_KEY_RIGHT] = Key::ARROWRIGHT;
   map[GLFW_KEY_UP] = Key::ARROWUP;

   // Special keys
   map[GLFW_KEY_ENTER] = Key::ENTER;
   map[GLFW_KEY_ESCAPE] = Key::ESCAPE;
   map[GLFW_KEY_TAB] = Key::TAB;
   map[GLFW_KEY_SPACE] = Key::SPACE;

   // Character mapping for direct input
   map['A'] = Key::A;
   map['B'] = Key::B;
   map['C'] = Key::C;
   map['D'] = Key::D;
   map['E'] = Key::E;
   map['F'] = Key::F;
   map['G'] = Key::G;
   map['H'] = Key::H;
   map['I'] = Key::I;
   map['J'] = Key::J;
   map['K'] = Key::K;
   map['L'] = Key::L;
   map['M'] = Key::M;
   map['N'] = Key::N;
   map['O'] = Key::O;
   map['P'] = Key::P;
   map['Q'] = Key::Q;
   map['R'] = Key::R;
   map['S'] = Key::S;
   map['T'] = Key::T;
   map['U'] = Key::U;
   map['V'] = Key::V;
   map['W'] = Key::W;
   map['X'] = Key::X;
   map['Y'] = Key::Y;
   map['Z'] = Key::Z;
}

auto GlfwWindow::width() -> u32 { return _width; }

auto GlfwWindow::height() -> u32 { return _height; }

auto GlfwWindow::set_width(u32 new_width) -> void {
   _width = new_width;
   if (!_window) {
      return;
   }
   glfwSetWindowSize(_window, new_width, this->height());
}

auto GlfwWindow::set_height(u32 new_height) -> void {
   _height = new_height;
   if (!_window) {
      return;
   }
   glfwSetWindowSize(_window, this->width(), new_height);
}

auto GlfwWindow::set_mouse(i32 x, i32 y) -> void {
   if (!_window) {
      return;
   }
   glfwSetCursorPos(_window, x, y);
}

auto GlfwWindow::set_cursor(Cursor cur) -> void {
   if (!_window) {
      return;
   }
   if (cur == Cursor::NONE) {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
   } else {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
}

auto GlfwWindow::set_title(const char *title) -> void {
   _title = title;
   if (!_window) {
      return;
   }
   glfwSetWindowTitle(_window, title);
}

auto GlfwWindow::ticks() -> u32 {
   return static_cast<u32>(glfwGetTime() * 1000.0); // Convert to milliseconds
}

auto GlfwWindow::title() -> const char * { return _title; }

auto GlfwWindow::swap_buffer() -> void { glfwSwapBuffers(_window); }

auto GlfwWindow::start() -> void {
#if defined(WINDOW_USE_CHRONO)
   auto prev_time = std::chrono::high_resolution_clock::now();
#else
   double prev_time = glfwGetTime();
#endif

   assert(_window && "Should have a valid window before window.start()");
   while (!glfwWindowShouldClose(_window)) {
      glfwPollEvents();

#if defined(WINDOW_USE_CHRONO)
      auto current_time = std::chrono::high_resolution_clock::now();
      double delta_time = std::chrono::duration<double, std::chrono::seconds::period>(current_time - prev_time).count();
#else
      double current_time = glfwGetTime();
      double delta_time = current_time - prev_time;
#endif

      app->on_update(*this, delta_time);
      prev_time = current_time;

      glfwPollEvents();
      swap_buffer();
   }
}

auto GlfwWindow::exit() -> void { glfwSetWindowShouldClose(_window, GLFW_TRUE); }

auto GlfwWindow::set_fullscreen(bool fullscreen) -> void {
   if (fullscreen) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      glfwSetWindowMonitor(_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
   } else {
      glfwSetWindowMonitor(_window, nullptr, 100, 100, _width, _height, 0);
   }
}

// auto GlfwWindow::delay(u32 ms) -> void { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

auto GlfwWindow::delay(u32 ms) -> void {
   #if defined(_WIN32) || defined(__MINGW32__)
      // Mingw doesnt implement some chronos stuff
       Sleep(ms); // Use Sleep on Windows
   #else
      #if defined(WINDOW_USE_CHRONO)
          std::this_thread::sleep_for(std::chrono::milliseconds(ms));
      #else
      #endif
   #endif
}

} // namespace cyx
