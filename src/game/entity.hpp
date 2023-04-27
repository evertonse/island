#pragma once
#include "math/vec.hpp"
#include "utils/common.h"
#include <vector>

using namespace cyx;

namespace island {

    struct Volume{
        bool* data;
        u32 xdim, ydim, zdim;

        static Volume make(u32 xdim, u32 ydim, u32 zdim) {
            Volume volume;
            volume.xdim = xdim;
            volume.ydim = ydim;
            volume.zdim = zdim;
            volume.data = new bool[xdim * ydim * zdim];
            return volume;
        }

        const bool& operator()(u32 x, u32 y, u32 z) const {
            assert(x < xdim && y < ydim && z < zdim);
            return data[x + y*xdim + z*xdim*ydim];
        }

        bool& operator()(u32 x, u32 y, u32 z) {
            assert(x < xdim && y < ydim && z < zdim);
            return data[x + y*xdim + z*xdim*ydim];
        }

        static void destroy(Volume* self ){
            delete[] self->data;
        }
    };


    struct World {
        veci3 dimensions{40,40,10};
        u32 island_percent{80}; 
        u32 lake_percent{20}; 
        
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

    static Entity make_entity() {

    }


} // namespace cyx::island

