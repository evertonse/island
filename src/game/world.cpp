#pragma once
#include "game/world.hpp"
#include "world.hpp"

using namespace cyx;

namespace island {

    Volume Volume::make(i32 xdim, i32 ydim, i32 zdim) {
        Volume volume;
        volume.xdim = xdim;
        volume.ydim = ydim;
        volume.zdim = zdim;
        volume.data = new EntityType[xdim * ydim * zdim];
        for (size_t i = 0; i < xdim * ydim * zdim; i++) {
            volume.data[i] = EntityType::NONE;    
        }
        
        return volume;
    }

    const EntityType& Volume::operator()(i32 x, i32 y, i32 z) const {
        assert(x < xdim && y < ydim && z < zdim);
        return data[x + y*xdim + z*xdim*ydim];
    }

    EntityType& Volume::operator()(i32 x, i32 y, i32 z) {
        assert(x < xdim && y < ydim && z < zdim);
        return data[x + y*xdim + z*xdim*ydim];
    }

    bool Volume::on_bounds(i32 x, i32 y, i32 z) const {
        return (
                (x < xdim && y < ydim && z < zdim)
            &&  (x >= 0   && y >= 0   && z >= 0)
        );
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
                    sand_block->texture.load("assets/textures/sand3.jpg");
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

        this->water_level = (f32(lake_percent)/(island_percent+lake_percent))* volume.ydim;

        for (int x = 0; x < volume.xdim; x++) {
            for (int z = 0; z < volume.zdim; z++) {
                int height = std::round(simplex2(z,x, 8, 750.f, 0.593f)*volume.ydim);
                //int height = water_level;
                height_map[{x,z}] = height;
                // The free list is places bove water level that are
                // 1 unit above a block 
                if (height > water_level) {
                    free_list.push_back( new veci3(x, height ,z));
                }

                for (int y = height - 1; y >= 0; y--) {
                    EntityType type = EntityType::SAND_BLOCK; 
                    volume(x,y,z)   = type;
                    
                }
            }
        }

        int suffocated_count = 0;
        int not_suffocated_count = 0; 
        int total_count = 0;
        for (int x = 0; x < volume.xdim; x++) {
            for (int z = 0; z < volume.zdim; z++) {
                int height = height_map[{x,z}];
                for (int y = height - 1; y >= 0; y--) {
                    EntityType type = volume(x,y,z);

                    total_count ++; 
                    // Check if the entity is "suffocated" by other entities around it
                    // that way we save a bit when rendering
                    bool is_suffocated = true;
                            
                    for (const auto& neighbour : {
                        veci3{x,y+1,z},
                        veci3{x,y-1,z},

                        veci3{x,y,z+1},
                        veci3{x,y,z-1},

                        veci3{x+1,y,z},
                        veci3{x-1,y,z},
                    }) {

                        const vec3& other = neighbour;
                        if (!volume.on_bounds(neighbour.x, neighbour.y, neighbour.z)){
                            suffocated_count ++;
                            is_suffocated = false;
                            continue;
                        }

                        if (volume(neighbour.x, neighbour.y, neighbour.z) == EntityType::NONE) {
                            is_suffocated = false;
                            suffocated_count ++;
                            break;
                        }else {

                            not_suffocated_count ++;
                        }

                    }
                    if (is_suffocated || type == EntityType::NONE) {
                        continue; // skip creating the entity for this block
                    }
                    //
                    Entity e = Entity::make(type);
                    e.world_position = veci3(x,y,z);
                    e.transform.position = vec3(f32(x),f32(y),f32(z)) ;
                    entities.push_back(e);
                }
            }
        }


        std::cout << " suffocated  count "<< suffocated_count 
                  << " not_suffocated  count "<< not_suffocated_count 
                  << " total  count "<< total_count ;
        
          auto prepare_entites  = [&](u32 count, EntityType type, f32 scale, vec3 translation) {
            for (size_t i = 0; i < count ; i++) {
                if (free_list.empty()) {
                    continue;
                }
                veci3 pos = random_from_free_list();
                volume(pos.x,pos.y,pos.z)   = type;
                Entity e = Entity::make(type);
                e.world_position = pos;
                e.transform.scale = scale;
                e.transform.position  = vec3(pos.x, pos.y-0.5f, pos.z) + translation;

                e.transform.last_position =   e.transform.position;
                e.transform.new_position  =   e.transform.position;

                entities.push_back(e);
                if (type == EntityType::TERRESTRIAL1 || type == EntityType::TERRESTRIAL2) {
                    movable_entities.push_back(&(entities.back()));
                }
            }
        };

        prepare_entites(terrestrial1_count, EntityType::TERRESTRIAL1, 2.5f, {0,0,0.22f});
        prepare_entites(terrestrial2_count, EntityType::TERRESTRIAL2, 1.15f, {0,0,0});
        prepare_entites(plant1_count, EntityType::PLANT1, 1.9f, {0,0,0.0f});
        prepare_entites(plant2_count, EntityType::PLANT2, 1.9f, {0,0,0.0f});

        return *this;
    }

    void World::update_positions() {
        f32 time_interval = 0.015;
        
        for (auto e : movable_entities) {
            int x = e->world_position.x;
            int y = e->world_position.y;
            int z = e->world_position.z;
            for (const auto& neighbour : {
                    //veci3{x,y+1,z},// we ignore y direction for now
                    //veci3{x,y-1,z},// we ignore y direction for now
                    veci3{x,y,z+1},
                    veci3{x,y,z-1},

                    veci3{x+1,y,z},
                    veci3{x-1,y,z},
                })
            {
                if (!volume.on_bounds(neighbour.x, neighbour.y, neighbour.z)){
                    continue;
                }

                if ( 
                    //  Check if it is clear
                    volume(neighbour.x, neighbour.y, neighbour.z) == EntityType::NONE
                    &&
                    //Check if below is ground
                    volume(neighbour.x, neighbour.y-1, neighbour.z) == EntityType::SAND_BLOCK
                ) {
                    // Upodate the new world position for this entity
                    e->world_new_position = veci3(neighbour.x, neighbour.y, neighbour.z);
                    // We need to update the volume soon, might as well try now
                    volume(e->world_new_position.x, e->world_new_position.y, e->world_new_position.z) = e->type;
                    // Mark as free
                    volume(e->world_position.x, e->world_position.y, e->world_position.z) = EntityType::NONE;
                    e->transform.new_position = vec3((f32)neighbour.x, (f32)neighbour.y, (f32)neighbour.z);
                    break;
                }
            }
        }
    }

    void World::tick_positions(f32 dt) {
        persistent_data f32 timer = 0;
        persistent_data f32 time_to_interpolate = 1.0;

        timer += dt;
        f32 t = std::min(timer / time_to_interpolate, 1.0f);

        for(auto& e : movable_entities) {
            e->transform.position.z += dt;
            //e->transform.position = e->transform.last_position + (e->transform.new_position - e->transform.last_position) * t;
        }

        if (timer >= time_to_interpolate) {
            //std::cout << "timer " << timer;
            timer = 0;
            for(auto& e : movable_entities) {

                //std::cout << "e.type" << u31(e->type);
                e->transform.last_position = e->transform.new_position;
                update_positions();
            }
        }
    }
    veci3 World::random_from_free_list() {
        assert(! free_list.empty()); 
        int index = random_int(free_list.size()-1);
        auto iter  = std::next(free_list.begin(), index);
        //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
        veci3* pos = std::move(*iter);
        veci3 v = veci3(pos->x,pos->y,pos->z);
        free_list.erase(iter);
        delete pos;
        return v;
    }
} // namespace cyx::island
