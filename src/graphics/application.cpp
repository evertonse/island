#include "graphics/application.hpp"

namespace cyx {

#if defined (CYX_POINTER_BASED_APPLICATION)
    Application::Application() {
        this->on_create  = [](Window&) { };
        this->on_update  = [](Window&,f64 dt) { };
        this->on_event   = [](Window&,Event e){ };
        this->on_destroy = [](Window&) { };
    }
#else
    Application::Application() { }
    void Application::on_create(Window& win) { };
    void Application::on_update (Window& win,f64 dt) { assert(0 && " Subclass must implement this function"); };
    void Application::on_event  (Window& win,Event e){ };
    void Application::on_destroy(Window& win) { };
#endif
}