#pragma once
#include <vector>
#include <tuple>
#include <map>
#include <list>
#include "noise.h"

#include "graphics/model.hpp"
#include "math/vec.hpp"
#include "utils/common.h"
#include "math/useful.hpp"
#include "defaults.h"


using namespace cyx;

namespace island {

    enum class EntityType :u32 {
        NONE = 0,
        TERRAIN_BLOCK,
        WATER_BLOCK,
        SAND_BLOCK,
        PLANT1, PLANT2,
        TERRESTRIAL1, TERRESTRIAL2
    };

   

    // Class represents the Volume of the whole scene
    // which is divided in block 1x1 
    struct Volume{
        EntityType empty = EntityType::NONE;
        EntityType* data;
        u32 xdim, ydim, zdim;

        static Volume make(i32 xdim, i32 ydim, i32 zdim);
        static Volume copy(const Volume *other);

        const EntityType &operator()(veci3 v) const;
        const EntityType& operator()(i32 x, i32 y, i32 z) const;

        EntityType& operator()(i32 x, i32 y, i32 z) ;

        bool on_bounds(i32 x, i32 y, i32 z) const;

        bool on_bounds(veci3 v) const;

        veci3 dim();
        static void destroy(Volume *self);
        static void dump(Volume *self);
    };


    // Transformations about an entity
    struct Transform {
        vec3 position{0};
        vec3 new_position{0};
        vec3 last_position{0};


        vec3 orientation{0};
        vec3 new_orientation{0};
        vec3 last_orientation{0};

        f32 angle{0};
        f32 new_angle{0};
        f32 last_angle {0};

        f32  scale = 1.0f; 
        vec3 velocity{0.0};
        vec3 acceleration{0.0};

        bool active = false;
        static Transform difference(const Transform& t1, const Transform& t2);
    };


    // Info about and entity
    struct Entity {
        Transform  transform;
        veci3  world_position {0,0};
        veci3  world_new_position {0,0};
        u32 id;
        Model* model;
        EntityType type;
        
        static Entity make(EntityType type);
        static void destroy(Entity *self);
    };


    // World that is the scene and all it needs
    struct World {
        World(){};
        using Coord2D = std::tuple<int, int>;
        using HeightMap = std::map<Coord2D, int>;

        HeightMap height_map;
        std::list<veci3*> free_list;        

        veci3 dimensions{100, 30, 60};
        u32 island_percent{50}; 
        u32 lake_percent{50}; 
        
        u32 terrestrial1_count = 50;
        u32 terrestrial2_count = 50;

        u32 plant1_count = 16;
        u32 plant2_count = 10;

        u32 water_level = 0;

        Volume volume;

        std::vector<Entity> movable_entities;
        std::vector<Entity> entities;
        std::vector<Entity> water_entities;
        bool force_inline is_movable(const Entity&);

        void generate_volume();
        void generate_water();
        void force_inline tick_positions(f32 dt);
        void from_file(const std::string& filename);

    private:
        void tendency(const Entity& e, std::vector<veci3>& neighbours);
        void prepare_entities(u32 count, EntityType type, f32 scale, vec3 translation);
        void force_inline update_positions();
        veci3* random_from_free_list();
        static f32 neighbour2angle(const vec3& n);


    };
} // namespace cyx::island
