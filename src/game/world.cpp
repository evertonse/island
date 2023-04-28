#pragma once
#include "game/world.hpp"
#include "world.hpp"

using namespace cyx;

namespace island {

    Volume Volume::make(u32 xdim, u32 ydim, u32 zdim) {
        Volume volume;
        volume.xdim = xdim;
        volume.ydim = ydim;
        volume.zdim = zdim;
        volume.data = new bool[xdim * ydim * zdim];
        return volume;
    }

    const bool& Volume::operator()(u32 x, u32 y, u32 z) const {
        assert(x < xdim && y < ydim && z < zdim);
        return data[x + y*xdim + z*xdim*ydim];
    }

    bool& Volume::operator()(u32 x, u32 y, u32 z) {
        assert(x < xdim && y < ydim && z < zdim);
        return data[x + y*xdim + z*xdim*ydim];
    }

    void Volume::destroy(Volume* self ){
        delete[] self->data;
    }


    Entity Entity:: make(EntityType type) {
        persistent_data int id_count = 0;

        persistent_data Model* terrain_block = new Model;
        persistent_data Model* terrestrial1  = new Model;
        persistent_data Model* terrestrial2  = new Model;
        persistent_data Model* terrestrial3  = new Model;
        persistent_data Model* plant1 = new Model;
        persistent_data Model* plant2 = new Model;
        persistent_data bool terrain_block_loaded = false;
        persistent_data bool terrestrial1_loaded  = false;
        persistent_data bool terrestrial2_loaded  = false;
        persistent_data bool terrestrial3_loaded  = false;
        persistent_data bool plant1_loaded  = false;
        persistent_data bool plant2_loaded  = false;

        Entity e;
        e.id = id_count++;
        e.type = type;
        switch (type) {
            case EntityType::PLANT1: {
                if (plant1_loaded == false){
                    TripleBufferMesh::bamboo(&plant1->mesh);
                    plant1->texture.load("assets/textures/tex_bamboo.jpg");
                    plant1_loaded = true; 
                }
                e.model = plant1; 
                break;
            }
            case EntityType::PLANT2: {
                if (plant2_loaded == false){
                    TripleBufferMesh::plant1(&plant2->mesh);
                    plant2->texture.load("assets/textures/tex_vase_plant.jpg");
                    plant2_loaded = true; 
                }
                e.model = plant2; 
                break;
            }
            case EntityType::TERRAIN_BLOCK: {
                if (terrain_block_loaded == false){
                    TripleBufferMesh::cube(&terrain_block->mesh);
                    terrain_block->texture.load("assets/textures/wall.jpg");
                    terrain_block_loaded = true; 
                }
                e.model = terrain_block; 
                break;
            }
            case EntityType::TERRESTRIAL1: {
                if (terrestrial1_loaded == false){
                    TripleBufferMesh::goblin(&terrestrial1->mesh);
                    terrestrial1->texture.load("assets/textures/tex_goblin.png");
                    terrestrial1_loaded = true; 
                }
                e.model = terrestrial1; 
                break;
            }
            case EntityType::TERRESTRIAL2: {
                if (e.id  % 2 == 0) {
                    if (terrestrial2_loaded == false){
                        TripleBufferMesh::tiger(&terrestrial2->mesh);
                        terrestrial2->texture.load("assets/textures/tiger_white.png");
                        terrestrial2_loaded = true;
                    }
                    e.model = terrestrial2;
                } else {
                    if (terrestrial3_loaded == false){
                        TripleBufferMesh::tiger(&terrestrial3->mesh);
                        terrestrial3->texture.load("assets/textures/tiger_yellow.png");
                        terrestrial3_loaded = true;
                    }
                    e.model = terrestrial3;
                }
                break;
            }
            default: {
                assert(0 && "Unkown Entity Type");
            }
        }
        return e;
    }

    void Entity::destroy(Entity *self) {
        delete self->model;
    }
} // namespace cyx::island
