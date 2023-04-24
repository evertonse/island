
#define CYX_POINTER_BASED_APPLICATION
#include "cyx.hpp"
#include "cyx.cpp"

#include <string>

using namespace cyx;


auto main(int argc, char* argv[] ) ->  int {
    Application app;

    app.on_create = [](Window& win) {

    };

    app.on_update = [](Window& win, f64 dt) {
        win.set_title(std::to_string(dt).c_str());
    };

    app.on_event  = [](Window& win,Event e){ 
        persistent_data bool fullscren = false;

        if (e.type == EventType::KEYDOWN) {
            if (e.key == Key::ESCAPE){
                fullscren = !fullscren; 
            }
        }
        win.set_fullscreen(fullscren);

    };

    Window* win = new GlutWindow(&app);

    win->start();

    return 0;
}