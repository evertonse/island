#pragma once
#include "math/vec.hpp"
#include "utils/common.h"

using namespace cyx;

namespace island {
    struct World {
        using Volume = std::vector<std::vector<std::vector<bool>>>;
        veci3 dimensions;
        u32 island_percent; 
        u32 lake_percent; 
        
        u32 terrestrial1_count;
        u32 terrestrial2_count;

        u32 plant1_count;
        u32 plant2_count;
        Volume vol;
    };

    struct Transform {
        vec3 position{0.0f};
        vec3 orientation{0.0f};

        vec3 last_position{0.0f};
        vec3 last_rotation{0.0f};

        vec3 velocity{0.0};

        bool active = false;
    };

    enum class EntityType {
        PLANT1, PLANT2,
        TERRESTRIAL1, TERRESTRIAL2
    };

    struct Entity {
        Transform  transform;
        veci3  world_position;
        u32 id;
        EntityType type;
    };


} // namespace cyx::island

