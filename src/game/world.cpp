#pragma once
#include "game/world.hpp"

using namespace cyx;

namespace island {

    Volume Volume::make(i32 xdim, i32 ydim, i32 zdim) {
        Volume volume;
        volume.xdim = xdim;
        volume.ydim = ydim;
        volume.zdim = zdim;
        volume.data = new EntityType[xdim * ydim * zdim];
        return volume;
    }

    const EntityType& Volume::operator()(i32 x, i32 y, i32 z) const {
        assert(x < xdim && y < ydim && z < zdim);
        x = clamp(x, 0, (int)xdim-1); // clamp x to [0, xdim-1]
        y = clamp(y, 0, (int)ydim-1); // clamp y to [0, ydim-1]
        z = clamp(z, 0, (int)zdim-1); // clamp z to [0, zdim-1]
        return data[x + y*xdim + z*xdim*ydim];
    }

    EntityType& Volume::operator()(i32 x, i32 y, i32 z) {
        assert(x < xdim && y < ydim && z < zdim);
        x = clamp(x,0, (int)xdim-1); // clamp x to [0, xdim-1]
        y = clamp(y,0, (int)ydim-1); // clamp y to [0, ydim-1]
        z = clamp(z,0, (int)zdim-1); // clamp z to [0, zdim-1]
        return data[x + y*xdim + z*xdim*ydim];
    }

    void Volume::destroy(Volume* self ){
        delete[] self->data;
    }


    Entity Entity:: make(EntityType type) {
        persistent_data int id_count = 0;
        persistent_data Model* terrain_block = new Model;
        persistent_data Model* sand_block = new Model;
        persistent_data Model* terrestrial1  = new Model;
        persistent_data Model* terrestrial2  = new Model;
        persistent_data Model* terrestrial3  = new Model;
        persistent_data Model* terrestrial4  = new Model;
        persistent_data Model* plant1 = new Model;
        persistent_data Model* plant2 = new Model;
        persistent_data bool terrain_block_loaded = false;
        persistent_data bool sand_block_loaded = false;
        persistent_data bool terrestrial1_loaded  = false;
        persistent_data bool terrestrial2_loaded  = false;
        persistent_data bool terrestrial3_loaded  = false;
        persistent_data bool terrestrial4_loaded  = false;
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
            case EntityType::SAND_BLOCK: {
                if (sand_block_loaded == false){
                    TripleBufferMesh::cube(&sand_block->mesh);
                    sand_block->texture.load("assets/textures/sand.jpg");
                    sand_block_loaded = true; 
                }
                e.model = sand_block; 
                break;
            }
            case EntityType::TERRESTRIAL1: {
                if (e.id  % 2 == 0) {
                    if (terrestrial4_loaded == false){
                        TripleBufferMesh::goblin(&terrestrial4->mesh);
                        terrestrial4->texture.load("assets/textures/tex_goblin.png");
                        terrestrial4_loaded = true;
                    }
                    e.model = terrestrial4;
                } else {
                    if (terrestrial1_loaded == false){
                        TripleBufferMesh::enemy(&terrestrial1->mesh);
                        terrestrial1->texture.load("assets/textures/enemy.png");
                        terrestrial1_loaded = true;
                    }
                    e.model = terrestrial1;
                }
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




    World& World::generate_volume() {
        this->volume = Volume::make(dimensions.x, dimensions.y, dimensions.z);
        for (int x = 0; x < volume.xdim; x++) {
            for (int z = 0; z < volume.zdim; z++) {
                int height = std::round(simplex2(z,x, 3, 50.f, 0.193f)*volume.ydim);

                // The free list is places bove water level that are
                // 1 unit above a block 
                if (height > water_level) {
                    free_list.emplace_back(x, height ,z);
                    std::cout 
                        << "free_list added : x:" <<x 
                        << " y:" << height 
                        << " z:" << z 
                        << " size:" << free_list.size()
                        << '\n';
                }
                else {
                    height_map[{x,z}] = height;
                }

                for (int y = height - 1; y >= 0; y--) {
                    EntityType type = EntityType::SAND_BLOCK; 
                    volume(x,y,z)   = type;
                    Entity e = Entity::make(type);
                    e.world_position = veci3(x,y,z);
                    e.transform.position = vec3(f32(x),f32(y),f32(z)) ;
                    entities.push_back(e);
                }
               
            }
        }
        std::cout << "terrestrial1_count" << terrestrial1_count;

        for (auto& item : free_list) {
            std::cout 
                << "free_list rfor: x:" <<item.x 
                << " y:" << item.y 
                << " z:" << item.z 
                << " size:" << free_list.size()
                << '\n';
        }

        for (size_t i = 0; i < terrestrial1_count; i++) {
            EntityType type = EntityType::TERRESTRIAL1; 
            if (free_list.empty()) {
                continue;
            }
            free_list.pop_back();
            //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
            veci3 pos = free_list[free_list.size()-1];
            std::cout
                    << "free_list terrestrial1: x:" << pos.x 
                    << " y:" << pos.y 
                    << " z:" << pos.z
                    << " size:" << free_list.size()
                    << '\n';

            volume(pos.x,pos.y,pos.z)   = type;
            Entity e = Entity::make(type);
            e.world_position = pos;
            e.transform.scale = 2.5f;
            e.transform.position = vec3(pos.x, pos.y-0.5f, pos.z+0.22f) ;
            entities.push_back(e);
            /* code */
        }
        
        for (size_t i = 0; i < terrestrial2_count; i++) {
            EntityType type = EntityType::TERRESTRIAL2; 
            if (free_list.empty()) {
                continue;
            }
            free_list.pop_back();
            //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
            veci3 pos = free_list[free_list.size()-1];
            std::cout
                    << "free_list terrestrial2: x:" << pos.x 
                    << " y:" << pos.y 
                    << " z:" << pos.z
                    << " size:" << free_list.size()
                    << '\n';

            volume(pos.x,pos.y,pos.z)   = type;
            Entity e = Entity::make(type);
            e.world_position = pos;
            e.transform.scale = 1.35f;
            e.transform.position = vec3(pos.x, pos.y-0.5f, pos.z+0.22f) ;
            entities.push_back(e);
            /* code */
        }

        for (size_t i = 0; i < plant1_count; i++) {
            EntityType type = EntityType::PLANT1; 
            if (free_list.empty()) {
                continue;
            }

            free_list.pop_back();
            //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
            veci3 pos = free_list[free_list.size()-1];
            volume(pos.x,pos.y,pos.z) = type;

            Entity e = Entity::make(type);
            e.world_position = pos;
            e.transform.scale = 1.9f;
            e.transform.position = vec3(pos.x, pos.y-0.5f, pos.z) ;
            entities.push_back(e);
            /* code */
        }

        for (size_t i = 0; i < plant2_count; i++) {
            EntityType type = EntityType::PLANT2; 
            if (free_list.empty()) {
                continue;
            }

            free_list.pop_back();
            //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
            veci3 pos = free_list[free_list.size()-1];
            volume(pos.x,pos.y,pos.z) = type;

            Entity e = Entity::make(type);
            e.world_position = pos;
            e.transform.scale = 1.9f;
            e.transform.position = vec3(pos.x, pos.y-0.5f, pos.z) ;
            entities.push_back(e);
            /* code */
        }

        return *this;
    }
} // namespace cyx::island
