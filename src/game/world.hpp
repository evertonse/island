#pragma once
#include <vector>

#include "graphics/model.hpp"
#include "math/vec.hpp"
#include "utils/common.h"

#define ISLAND_CUBE_VOLUME 1.0

using namespace cyx;

namespace island {

    enum class EntityType {
        TERRAIN_BLOCK,
        SAND_BLOCK,
        WATER_BLOCK,
        PLANT1, PLANT2,
        TERRESTRIAL1, TERRESTRIAL2
    };

    struct Volume{
        EntityType* data;
        u32 xdim, ydim, zdim;

        static Volume make(u32 xdim, u32 ydim, u32 zdim);

        const EntityType& operator()(u32 x, u32 y, u32 z) const;

        EntityType& operator()(u32 x, u32 y, u32 z) ;

        static void destroy(Volume* self );
    };


    struct Transform {
        vec3 position{0.0f};
        vec3 orientation{0.0f};

        vec3 last_position{0.0f};
        vec3 last_rotation{0.0f};

        vec3 velocity{0.0};
        vec3 acceleration{0.0};

        bool active = false;
    };


    struct Entity {
        Transform  transform;
        veci3  world_position {0,0};
        u32 id;
        Model* model;
        EntityType type;
        

        static Entity make(EntityType type);
        static void destroy(Entity *self);
    };

    struct World {
        veci3 dimensions{10,10,10};

        u32 island_percent{80}; 
        u32 lake_percent{20}; 
        
        u32 terrestrial1_count;
        u32 terrestrial2_count;

        u32 plant1_count;
        u32 plant2_count;

        u32 water_level = 2;

        Volume volume;
        std::vector<Entity> entities;

        World& generate_volume();
    };
} // namespace cyx::island

