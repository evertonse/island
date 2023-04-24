#pragma once
#include "utils/common.h"
#include "math/vec.hpp"
#include "math/useful.hpp"
#include "graphics/event.hpp"
#include "graphics/window.hpp"
#include "game/world/entity.hpp"

using namespace cyx;
namespace island {
    class Keyboard;
    class ClientWorld;

    class Player {
    public:
        void tick(f32 dt);

        EntityState state;
        u32 player_id;

        void on_keydown(Key key);
        //void on_mouse(Window& win,int x, int y);
        void on_mouse(Window& win, int x, int y,bool lock_mouse);
        veci2 last_mouse;
    private:

        vec3 acceleration;
        vec3 velocity;
        bool flying = false;
    };

} // namespace cyx