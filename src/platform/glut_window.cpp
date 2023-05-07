#include "platform/glut_window.hpp"
#include "glut_window.hpp"

namespace cyx {


    Application* GlutWindow::curr_app = nullptr;
    GlutWindow* GlutWindow::curr_win = nullptr;
    Event GlutWindow::event = Event{};
  	std::map<size_t, Key> GlutWindow::map = std::map<size_t, Key>();

    // Window class implementation using GLUT
    GlutWindow::GlutWindow(Application* app, const char* title, int width, int height) {
        this->app = app;
        GlutWindow::curr_app = app;
        GlutWindow::curr_win = this;
        int argc = 0;
        char* argv[1] = { (char*)"" };
        glutInit(&argc, argv);
        this->app->on_init(*this); // OBS:  can't call gl functions from here
        
        auto flags = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH;
        // If you want MultiSample, uncomment the below
        flags = flags | GLUT_MULTISAMPLE;
        glutInitDisplayMode(flags);
        glutInitWindowPosition(
            (glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
            (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2
        );
        glutInitWindowSize(width, height);
        this->_width = width;
        this->_height = height;
        
        glutCreateWindow(title);
        this->app->on_create(*this);

        //gladLoadGL();
        glutDisplayFunc( [](){
            auto& app = GlutWindow::curr_app;
            auto win = GlutWindow::curr_win;

        #if defined(WINDOW_USE_CHRONO)
            persistent_data auto prev_time = std::chrono::high_resolution_clock::now();
            // Get the current time
            auto current_time = std::chrono::high_resolution_clock::now();
            f64 delta_time = 
                std::chrono::duration<f64,std::chrono::seconds::period>(current_time - prev_time).count();
        #else
            persistent_data double prev_time = 0.0;
            // Get the current time
            double current_time = static_cast<double>(glutGet(GLUT_ELAPSED_TIME));
            // Divide this by 1000.0f to calculate delta time in seconds
            double delta_time = (current_time - prev_time) / 1000.0;

            // Update the previous time
        #endif

            app->on_update(*win, delta_time);
            prev_time = current_time;
            // We could make swap buffer as a default, but I prefer leaving the user
            // to choose if wants to swap the buffer or not. 
            // (if you want you could uncomment the next lien)
            //glutSwapBuffers();
            glutPostRedisplay();

        });

        map_keys();
        glutKeyboardFunc([](unsigned char key, int x, int y) -> void {
            auto&& app = GlutWindow::curr_app;
            auto win = GlutWindow::curr_win;
            Event& e = GlutWindow::event;
            e.type = EventType::KEYDOWN;
            switch (glutGetModifiers()) {
                case 0: 
                    if ('a' <= key && key <= 'z') key -= 32;
                    break;
                case GLUT_ACTIVE_SHIFT:
                    e.key = Key::SHIFT;
                    app->on_event(*win,e);
                    return;
                case GLUT_ACTIVE_CTRL:
                    if ('a' <= key && key <= 'z') key -= 32;
                    e.key = Key::CTRL;
                    app->on_event(*win,e);
                    return;
                case GLUT_ACTIVE_ALT:
                    if ('a' <= key && key <= 'z') key -= 32;
                    break;
            }
            e.key = map[key];
            app->on_event(*win,e);
        });

        glutKeyboardUpFunc([]( unsigned char key, int x, int y) {
            auto&& app = GlutWindow::curr_app;
            Event& e = GlutWindow::event;
            e.type = EventType::KEYUP;
            auto win = GlutWindow::curr_win;
            switch (glutGetModifiers()) {
                case 0: 
                    if ('a' <= key && key <= 'z') key -= 32;
                    break;
                case GLUT_ACTIVE_SHIFT:
                    e.key = Key::SHIFT;
                    app->on_event(*win,e);
                    break;
                case GLUT_ACTIVE_CTRL:
                    if ('a' <= key && key <= 'z') key -= 32;
                    e.key = Key::CTRL;
                    app->on_event(*win,e);
                    break;
                case GLUT_ACTIVE_ALT:
                    if ('a' <= key && key <= 'z') key -= 32;
                    break;
            }
            e.key = map[key];
            app->on_event(*win,e);
        });

        glutReshapeFunc([](int width, int height) {
            auto&& app = GlutWindow::curr_app;
            auto win = GlutWindow::curr_win;
            Event& e = GlutWindow::event;
            win->_width = width; 
            win->_height = height; 
            e.type = EventType::RESHAPE;
            app->on_event(*win,e);
        });

        glutMouseFunc( [](int button, int state, int x, int y) -> void {
            auto&& app = GlutWindow::curr_app;
            auto win = GlutWindow::curr_win;
            Event& e = GlutWindow::event;
            e.type = EventType::NONE;
            persistent_data int lastx = x;
            persistent_data int lasty = y;
            if (state == GLUT_UP) {
                e.type = EventType::MOUSEUP; 
            }
            else if (state == GLUT_DOWN) {
                e.type = EventType::MOUSEDOWN; 
            }

            switch (button) {
                case GLUT_LEFT_BUTTON:
                    e.mouse.button = MouseButton::LEFT;
                    break;
                case GLUT_MIDDLE_BUTTON:
                    e.mouse.button = MouseButton::MIDDLE;
                    break;
                case GLUT_RIGHT_BUTTON:
                    e.mouse.button = MouseButton::RIGHT;
                    break;
                case 3:
                    e.type = EventType::MOUSEWHEEL;
                    e.mouse.wheel.dir = +1;
                    break;
                case 4:
                    e.type = EventType::MOUSEWHEEL;
                    e.mouse.wheel.dir = -1 ;
                    break;
            }
            lastx = x;
            lasty = y;
            app->on_event(*win,e);
        });

        glutMotionFunc([](int x, int y){
            auto&& app = GlutWindow::curr_app;
            Event& e = GlutWindow::event;
            auto win = GlutWindow::curr_win;
            e.type = EventType::NONE;
            e.mouse.x = x;
            e.mouse.y = y;
            e.type = EventType::MOUSEMOVE;
            app->on_event(*win,e);
        });

        glutPassiveMotionFunc([](int x, int y){
            auto&& app = GlutWindow::curr_app;
            Event& e = GlutWindow::event;
            auto win = GlutWindow::curr_win;
            e.type = EventType::NONE;
            e.mouse.x = x;
            e.mouse.y = y;
            e.type = EventType::MOUSEMOVE;
            app->on_event(*win,e);
            e.type = EventType::NONE;
        });

        glutSpecialFunc([](int key, int x, int y) {
            auto&& app = GlutWindow::curr_app;
            Event& e = GlutWindow::event;
            e.type == EventType::KEYDOWN;
            auto win = GlutWindow::curr_win;
            switch (key)
            {
                case GLUT_KEY_LEFT:
                    e.key = Key::ARROWLEFT;
                    break;
                case GLUT_KEY_UP:
                    e.key = Key::ARROWUP;
                    break;
                case GLUT_KEY_RIGHT:
                    e.key = Key::ARROWRIGHT;
                    break;
                case GLUT_KEY_DOWN:
                    e.key = Key::ARROWDOWN;
                    break;
                default:
                    break;
            }
            app->on_event(*win,e);
        });
    }
    
    GlutWindow::~GlutWindow() {
        glutDestroyWindow(glutGetWindow());;
    }

    auto GlutWindow::map_keys() -> void {
		map['A'] = Key::A; map['B'] = Key::B; map['C'] = Key::C; map['D'] = Key::D; map['E'] = Key::E;
		map['F'] = Key::F; map['G'] = Key::G; map['H'] = Key::H; map['I'] = Key::I; map['J'] = Key::J;
		map['K'] = Key::K; map['L'] = Key::L; map['M'] = Key::M; map['N'] = Key::N; map['O'] = Key::O;
		map['P'] = Key::P; map['Q'] = Key::Q; map['R'] = Key::R; map['S'] = Key::S; map['T'] = Key::T;
		map['U'] = Key::U; map['V'] = Key::V; map['W'] = Key::W; map['X'] = Key::X; map['Y'] = Key::Y;
		map['Z'] = Key::Z;

        map[GLUT_KEY_F1] = Key::F1; map[GLUT_KEY_F2] = Key::F2; map[GLUT_KEY_F3] = Key::F3; map[GLUT_KEY_F4] = Key::F4;
        map[GLUT_KEY_F5] = Key::F5; map[GLUT_KEY_F6] = Key::F6; map[GLUT_KEY_F7] = Key::F7; map[GLUT_KEY_F8] = Key::F8;
        map[GLUT_KEY_F9] = Key::F9; map[GLUT_KEY_F10] = Key::F10; map[GLUT_KEY_F11] = Key::F11; map[GLUT_KEY_F12] = Key::F12;

        map[GLUT_KEY_DOWN] = Key::ARROWDOWN; map[GLUT_KEY_LEFT] = Key::ARROWLEFT; map[GLUT_KEY_RIGHT] = Key::ARROWRIGHT; map[GLUT_KEY_UP] = Key::ARROWUP;
        map[13] = Key::ENTER;

        map[27] = Key::ESCAPE;
        map[9] = Key::TAB;
        map[32] = Key::SPACE;
    }


    auto GlutWindow::width() -> u32 {
        //return glutGet(GLUT_WINDOW_WIDTH);
        return _width;
    }

    auto GlutWindow::height() -> u32 {
        return _height;
    }

    auto GlutWindow::set_width(u32 new_width) -> void {
        _width = new_width;
        glutReshapeWindow(new_width, this->height());
    }

    auto GlutWindow::set_height(u32 new_height) -> void {
        _height = new_height;
        glutReshapeWindow(this->width(),new_height);
    }

    auto GlutWindow::set_mouse(i32 x, i32 y) -> void {
        glutWarpPointer(x, y);
    }
    
    auto GlutWindow::set_cursor(Cursor cur) -> void {
        if (cur ==  Cursor::NONE)
            glutSetCursor(GLUT_CURSOR_NONE);
        else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
    }

    auto GlutWindow::set_title(const char* title) -> void {
        _title = title;
        glutSetWindowTitle(title);
    }

    auto GlutWindow::ticks() -> u32 {
        return glutGet(GLUT_ELAPSED_TIME);
    }

    auto GlutWindow::title() -> const char* {
        return _title;
    }

    auto GlutWindow::swap_buffer() -> void {
        glutSwapBuffers();
    }

    auto GlutWindow::start() -> void {
        glutMainLoop();
    }


    auto GlutWindow::exit() -> void {
        assert(0&& 
            "Glut does not support leaving Main Loop without exiting whole program" 
            "with that said, you might consider using FreeGLUT which supports"
            "glutLeaveMainLoop ( void );"
        );
    }
    auto GlutWindow::set_fullscreen(bool fullscreen) -> void {
        if (fullscreen) {
            glutFullScreen();
        } 
        else{
            glutReshapeWindow(this->_width, this->_height);
        }
    }
    auto  GlutWindow::delay(u32 ms) -> void {
        persistent_data auto stub = [](int){};
        glutTimerFunc(ms, stub, 0);
    }
}