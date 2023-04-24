#pragma once
#include "math/vec.hpp"

using namespace cyx;
namespace island {

    struct EntityState {
        vec3 position{0.0f};
        vec3 rotation{0.0f};

        vec3 last_position{0.0f};
        vec3 last_rotation{0.0f};

        vec3 velocity{0.0};

        bool active = false;
    };


} // namespace cyx::island

