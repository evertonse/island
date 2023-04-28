#pragma once
#include <vector>
#include <tuple>
#include <map>

#include "noise.h"

#include "graphics/model.hpp"
#include "math/vec.hpp"
#include "utils/common.h"
#include "math/useful.hpp"

#define ISLAND_CUBE_VOLUME 2.0


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

        static Volume make(i32 xdim, i32 ydim, i32 zdim);

        const EntityType& operator()(i32 x, i32 y, i32 z) const;

        EntityType& operator()(i32 x, i32 y, i32 z) ;

        static void destroy(Volume* self );
    };


    struct Transform {
        vec3 position{0.0f};
        vec3 orientation{0.0f};

        vec3 last_position{0.0f};
        vec3 last_rotation{0.0f};

        vec3 new_position{0.0f};
        vec3 new_rotation{0.0f};
        f32 scale = 1.0f; 
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
        World(){};
        using Coord2D = std::tuple<int, int>;
        using HeightMap = std::map<Coord2D, int>;

        HeightMap height_map;
        std::vector<veci3> free_list;        

        veci3 dimensions{10,10,10};
        u32 island_percent{80}; 
        u32 lake_percent{20}; 
        
        u32 terrestrial1_count = 20;
        u32 terrestrial2_count = 20;

        u32 plant1_count = 10;
        u32 plant2_count = 10;

        u32 water_level = 1;

        Volume volume;
        std::vector<Entity> entities;

        World& generate_volume();
        void update_positions();
    };
} // namespace cyx::island

