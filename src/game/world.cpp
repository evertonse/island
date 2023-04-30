#pragma once
#include "game/world.hpp"
#include "world.hpp"

using namespace cyx;

namespace island {

    Transform Transform::difference(const Transform &t1, const Transform &t2) {
        Transform diff;
        diff.position = t1.position - t2.position;
        diff.new_position = t1.new_position - t2.new_position;
        diff.last_position = t1.last_position - t2.last_position;

        diff.orientation = t1.orientation - t2.orientation;
        diff.new_orientation = t1.new_orientation - t2.new_orientation;
        diff.last_orientation = t1.last_orientation - t2.last_orientation;

        diff.scale = t1.scale - t2.scale;
        diff.velocity = t1.velocity - t2.velocity;
        diff.acceleration = t1.acceleration - t2.acceleration;
        diff.active = t1.active != t2.active;
        return diff;
    }

    std::ostream& operator<<(std::ostream& os, const Transform& transform) {
        auto close = [](f32 value) -> f32 {
            constexpr f32 tolerance = 0.001f;
            if (std::abs(value) < tolerance) {
                return 0.0f;
            }
            return value;
        };

        os << "\nPosition: (" << close(transform.position.x) << ", " << close(transform.position.y) << ", " << close(transform.position.z) << ") ";
        os << "\nLast Position: (" << close(transform.last_position.x) << ", " << close(transform.last_position.y) << ", " << close(transform.last_position.z) << ") ";
        os << "\nNew Position: (" << close(transform.new_position.x) << ", " << close(transform.new_position.y) << ", " << close(transform.new_position.z) << ") ";
        os << "\nOrientation: (" << close(transform.orientation.x) << ", " << close(transform.orientation.y) << ", " << close(transform.orientation.z) << ") ";
        os << "\nLast Orientation: (" << close(transform.last_orientation.x) << ", " << close(transform.last_orientation.y) << ", " << close(transform.last_orientation.z) << ") ";
        os << "\nNew Orientation: (" << close(transform.new_orientation.x) << ", " << close(transform.new_orientation.y) << ", " << close(transform.new_orientation.z) << ") ";
        os << "\nScale: " << close(transform.scale) << " ";
        os << "\nVelocity: (" << close(transform.velocity.x) << ", " << close(transform.velocity.y) << ", " << close(transform.velocity.z) << ") ";
        os << "\nAcceleration: (" << close(transform.acceleration.x) << ", " << close(transform.acceleration.y) << ", " << close(transform.acceleration.z) << ") ";
        os << "\nActive: " << (transform.active ? "true" : "false");

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const EntityType& entity_type) {
        switch(entity_type) {
            case EntityType::NONE: 
                os << "NONE";
                break;
            case EntityType::TERRAIN_BLOCK:
                os << "TERRAIN_BLOCK";
                break;
            case EntityType::SAND_BLOCK:
                os << "SAND_BLOCK";
                break;
            case EntityType::WATER_BLOCK:
                os << "WATER_BLOCK";
                break;
            case EntityType::PLANT1:
                os << "PLANT1";
                break;
            case EntityType::PLANT2:
                os << "PLANT2";
                break;
            case EntityType::TERRESTRIAL1:
                os << "TERRESTRIAL1";
                break;
            case EntityType::TERRESTRIAL2:
                os << "TERRESTRIAL2";
                break;
        }
        return os;
    }


    std::ostream& operator<<(std::ostream& os, const Entity& e) {
        os 
            << "\nPosition " << e.transform.position
            << "\nOrientation " << e.transform.orientation
            //<< "\nLast_position " << e.transform.last_position
            //<< "\nNew_position " << e.transform.new_position
            << "\nWorld Position "     << e.world_position
            << "\nNew World Position " << e.world_new_position
        ;
        return os;
    }



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

    const EntityType& Volume::operator()(veci3 v) const {

        assert( (v.x < xdim && v.y < ydim && v.z < zdim)
            &&  (v.x >= 0   && v.y >= 0   && v.z >= 0));
        return data[v.x + v.y*xdim + v.z*xdim*ydim];
    }

    const EntityType& Volume::operator()(i32 x, i32 y, i32 z) const {

        assert( (x < xdim && y < ydim && z < zdim)
            &&  (x >= 0   && y >= 0   && z >= 0));
        return data[x + y*xdim + z*xdim*ydim];
    }

    EntityType& Volume::operator()(i32 x, i32 y, i32 z) {
        assert( (x < xdim && y < ydim && z < zdim)
            &&  (x >= 0   && y >= 0   && z >= 0));
        return data[x + y*xdim + z*xdim*ydim];
    }

    bool Volume::on_bounds(i32 x, i32 y, i32 z) const {
        return (
                x <  xdim && y <  ydim && z <  zdim
            &&  x >= 0    && y >= 0    && z >= 0
        );
    }

    bool Volume::on_bounds(veci3 v) const {
        return  on_bounds(v.x,v.y,v.z);    
    }

    veci3 Volume::dim() {
        return veci3(xdim,ydim, zdim);
    }

    void Volume::destroy(Volume *self) {
        delete[] self->data;
    }

    void Volume::dump(Volume* self ){
        std::ofstream file("volume_dump.txt");
        for (int z = 0; z < self->zdim; z++) {
            for (int y = 0; y < self->ydim; y++) {
                for (int x = 0; x < self->xdim; x++) {
                    int index = x + y * self->xdim + z * self->xdim * self->ydim;
                    const auto value = self->data[index];
                    vec3 position = vec3(x, y, z);
                    file << position << " : " << u32(value) << "\n";
                }
            }
        }

        file.close();
    }

   Volume Volume::copy(const Volume* other) {
        Volume volume;
        volume.xdim = other->xdim;
        volume.ydim = other->ydim;
        volume.zdim = other->zdim;
        volume.data = new EntityType[other->xdim * other->ydim * other->zdim];
        for (size_t i = 0; i < other->xdim * other->ydim * other->zdim; i++) {
            volume.data[i] = other->data[i];
        }
        return volume;
    }


    Entity Entity:: make(EntityType type) {
        persistent_data int id_count = 0;
        persistent_data Model* terrain_block = new Model;
        persistent_data Model* sand_block = new Model;
        persistent_data Model* water_block = new Model;
        persistent_data Model* terrestrial1  = new Model;
        persistent_data Model* terrestrial2  = new Model;
        persistent_data Model* terrestrial3  = new Model;
        persistent_data Model* terrestrial4  = new Model;
        persistent_data Model* plant1 = new Model;
        persistent_data Model* plant2 = new Model;
        persistent_data bool terrain_block_loaded = false;
        persistent_data bool water_block_loaded = false;
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
            case EntityType::WATER_BLOCK: {
                if (water_block_loaded == false){
                    TripleBufferMesh::cube(&water_block->mesh);
                    water_block->texture.load("assets/textures/water3.png");
                    water_block_loaded = true; 
                }
                e.model = water_block; 
                break;
            }  
            case EntityType::SAND_BLOCK: {
                if (sand_block_loaded == false){
                    TripleBufferMesh::cube(&sand_block->mesh);
                    sand_block->texture.load("assets/textures/sand3.jpg");
                    //sand_block->texture.load("assets/textures/sand.jpg");
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

    bool World::is_movable(const Entity & e) {
        if ( e.type == EntityType::TERRESTRIAL1 || e.type == EntityType::TERRESTRIAL2) {
            return true;
        }
        return false;
    }

    void World::generate_volume()
    {
        this->volume = Volume::make(dimensions.x, dimensions.y, dimensions.z);
        assert(island_percent+lake_percent == 100); 
        this->water_level = std::floor(((f32)(lake_percent)/(island_percent+lake_percent))* volume.ydim);
        std::cout << "Water level : " << water_level << '\n';

        // We need to make sure we know and reserve the max ammount of entities
        // because we take a pointer to this volume which means that this vector
        // should not EVER be resized, otherwise, you'll invalidate all
        // movable entities pointers;
        entities.reserve(volume.zdim *volume.ydim*volume.zdim);

        for (int x = 0; x < (int)volume.xdim; x++) {
            for (int z = 0; z < (int)volume.zdim; z++) {
                #if defined(ISLAND_FLAT_TERRAIN) && ISLAND_FLAT_TERRAIN != 0
                int height = water_level + 1;
                #else
                int height = std::round(simplex2(z,x, 9, 25000.0f, 0.609f)*volume.ydim);
                #endif
                height  = clamp(height,0,(int)volume.zdim);

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
        for (int x = 0; x < (int)volume.xdim; x++) {
            for (int z = 0; z < (int)volume.zdim; z++) {
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
                    e.world_new_position = veci3(e.world_position);
                    e.transform.position = vec3(f32(x),f32(y),f32(z)) ;
                    e.transform.last_orientation = vec3(e.transform.position);
                    e.transform.new_position     = vec3(e.transform.position);
                    entities.push_back(e);
                } 
            }
        }


        std::cout << " suffocated  count "<< suffocated_count 
                  << " not_suffocated  count "<< not_suffocated_count 
                  << " total  count "<< total_count ;
        
        prepare_entities(terrestrial1_count, EntityType::TERRESTRIAL1, 2.5f, {0,0,0.22f});
        prepare_entities(terrestrial2_count, EntityType::TERRESTRIAL2, 1.25f, {0,0,0});
        prepare_entities(plant1_count, EntityType::PLANT1, 4.8f, {0,0,0.0f});
        prepare_entities(plant2_count, EntityType::PLANT2, 3.8f, {0,0,0.0f});

        
        // wfree_list.clear();
        return;
    }

    void World::prepare_entities(u32 count, EntityType type, f32 scale, vec3 translation) {
        for (size_t i = 0; i < count ; i++) {
            if (free_list.empty()) {
                continue;
            }

            veci3* pos = random_from_free_list();
            assert(pos != nullptr  && "Position is null" );

            volume(pos->x,pos->y,pos->z)   = type;
            Entity e = Entity::make(type);
            e.world_position = veci3(*pos);
            e.world_new_position = veci3(*pos);
            e.transform.scale = scale;
            e.transform.position  =  vec3(pos->x, pos->y-0.5f, pos->z) + translation;
            e.transform.last_position =   vec3(e.transform.position);
            e.transform.new_position  =   vec3(e.transform.position);
            entities.push_back(e);

            // We assume entities won't be rezise so make sure of that @important
            int index = entities.size() -1;
            if (type == EntityType::TERRESTRIAL1 || type == EntityType::TERRESTRIAL2) {
                
                (&entities[index])->world_new_position.x = (entities[index]).world_position.x;
                (&entities[index])->world_new_position.y = (entities[index]).world_position.y;
                (&entities[index])->world_new_position.z = (entities[index]).world_position.z;
                (entities[index]).transform.orientation = vec3(0,0,1);
                movable_entities.push_back(&entities[index]);
            }
        }
    };


    // NOTE SWEATY FUNCTION to implemente, you were calling this in a tight loop
    // of movable entities, meaning it was being update n^2, which is obviously not correct
    void World::update_positions() {
        std::vector<std::tuple<veci3,EntityType>> updates; 
        bool first_time = true;

        for (auto e : movable_entities) {

            assert(e->type != EntityType::NONE 
                && e->type != EntityType::PLANT1
                && e->type != EntityType::PLANT2
                && e->type != EntityType::SAND_BLOCK
            );
    
            // Out last position sohuld now be the current position
            // Neighbours to decide the next possible position
            std::vector<veci3> neighbours = {
                veci3(1,0,0), veci3(-1,0,0),
                veci3(0,0,-1), veci3(0,0,+1),
                //veci3{x,y+1,z},
                //veci3{x,y-1,z},
            };
            int tendency = 6;
            for (int i = 0; i < tendency; i++) {
                neighbours.push_back(e->transform.orientation);
            }
             
            // Shuffle the next possible positions, but first add some probability of being
            // more likely to continue the the same direction
            shuffle(neighbours);

            e->world_position.x = e->world_new_position.x;
            e->world_position.y = e->world_new_position.y;
            e->world_position.z = e->world_new_position.z;
            for (const auto& neighbour : neighbours) {

                // Update the new world position for this entity
                int x = e->world_position.x + neighbour.x;
                int y = e->world_position.y + neighbour.y;
                int z = e->world_position.z + neighbour.z;
                
                bool on_bounds = (
                    x < dimensions.x && y <  dimensions.y && z <  dimensions.z
                 &&  x >= 0    && y >= 0    && z >= 0
                );
                if (!on_bounds){
                    continue;
                    std::cout << "volume.dim " << volume.dim();
                    std::cout << "world.dim " << dimensions;
                    std::cout  << e->transform << e->transform;
                }

                if (
                    //!volume.on_bounds(e->transform.position) ||
                    !volume.on_bounds(x,y,z)
                    ||
                    !volume.on_bounds(x,y-1,z)
                ){
                    continue;
                }
                #if 0 
                {
                    if (!volume.on_bounds(e->transform.position))
                        std::cout << *e << std::cin.get();
                    e->world_position = veci3(e->world_new_position);
                    e->world_new_position  =  veci3(e->world_position + neighbour);

                    e->transform.last_position = vec3(e->transform.position);
                    // remember to add -0.5f to y
                    
                    e->transform.new_position  = vec3(e->transform.position + neighbour);

                    break;
                }
                #endif
                if ( 
                    //  Check if it is clear
                    volume(x,y,z) == EntityType::NONE
                    &&
                    //Check if below is ground
                    volume(x,y-1,z) == EntityType::SAND_BLOCK
                ) {

                    // Old Position is now free
                    //volume(e->world_position) == EntityType::NONE;
                    // We're finished a tick so we are in new world position
                    // Mark as free
                    // Mark new ocupied place
                    volume(e->world_position.x,e->world_position.y, e->world_position.z) = e->type;
                    updates.push_back({
                        veci3(e->world_position.x, e->world_position.y, e->world_position.z),
                        EntityType::NONE 
                    }); 

                    e->world_new_position.x  =  e->world_position.x + neighbour.x;
                    e->world_new_position.y  =  e->world_position.y + neighbour.y;
                    e->world_new_position.z  =  e->world_position.z + neighbour.z;


                    e->transform.last_orientation = e->transform.orientation; 
                    e->transform.new_orientation = vec3(neighbour);

                    e->transform.last_angle = e->transform.angle; 
                    e->transform.new_angle = neighbour2angle(neighbour);
                    
                    e->transform.last_position = e->transform.position;
                    e->transform.new_position.x = (f32)e->transform.last_position.x + neighbour.x;
                    e->transform.new_position.y = (f32)e->transform.last_position.y + neighbour.y;
                    e->transform.new_position.z = (f32)e->transform.last_position.z + neighbour.z;
                    // We need to update the volume soon, might as well try now
                    volume(e->world_new_position.x, e->world_new_position.y, e->world_new_position.z) = e->type;
                    break;
                }
            }
        }  

        for (const auto& update : updates) {
            veci3 pos = std::get<0>(update);
            EntityType type = std::get<1>(update);
            volume(pos.x, pos.y, pos.z) = type;
        }
    }

    void World::tick_positions(f32 dt) {
        persistent_data f32 timer = 0;
        persistent_data constexpr f32 time_to_interpolate = 2.1;
        timer += dt;

        //f32 t = std::min(timer / time_to_interpolate, 1.0f);
        //f32 t = ease_in_out(timer / time_to_interpolate, 1.0f);
        f32 t = ease_in_out(std::min(timer / time_to_interpolate, 1.0f), 3.5f);

        for(auto& e : movable_entities) {
            e->transform.position = (vec3(e->transform.last_position)* (1.0f - t)) + (vec3(e->transform.new_position)* t);
            e->transform.orientation = (vec3(e->transform.last_orientation)* (1.0f - t)) + (vec3(e->transform.new_orientation)* t);
            e->transform.angle= e->transform.last_angle* (1.0f - t) + e->transform.new_angle * t;
        }
        if (timer >= time_to_interpolate) {
            timer = 0;
            update_positions();
        }
    }

    f32 World::neighbour2angle(const veci3& n) {
        if (n.x == 1 )
            return 90.0f; 
        else if (n.x == -1)
            return -90.0f; 
        else if (n.z == 1)
            return -0.0f;
        else if (n.z == -1)
            return 180.f;
        assert(0 && "this function should not be used outside the context of this");
    }

    veci3 *World::random_from_free_list()
    {
        assert(!free_list.empty() && "what is empty bro"); 
        if (free_list.empty()) {
            return nullptr;
        }
        int index = random_int(free_list.size()-1);
        auto&& iter  = std::next(free_list.begin(), index);
        //veci3 pos = free_list.back();// Cant use this becuase veci3 doenst implement certain operators
        veci3* pos = std::move(*iter);
        free_list.erase(iter);
        return pos;
    }
    // Has to be called after
    void World::generate_water() {
        water_entities.reserve(volume.xdim + (int)volume.zdim);
        for (int x = 0; x < (int)volume.xdim; x++) {
            for (int z = 0; z < (int)volume.zdim; z++) {
                for (
                    int y = clamp((int)water_level,0,(int)volume.zdim);
                    y >= 0;
                    y--
                ) {
                    if (volume(x,water_level,z) == EntityType::NONE
                        || x >= volume.xdim-1
                        || z >= volume.zdim-1
                        || x == 0
                        || z == 0
                    ) {
                        Entity e = Entity::make(EntityType::WATER_BLOCK);
                        e.world_position = veci3(x,y,z);
                        e.world_new_position = veci3(e.world_position);
                        e.transform.position = vec3(f32(x),f32(y),f32(z)) ;
                        e.transform.last_orientation = vec3(e.transform.position);
                        e.transform.new_position     = vec3(e.transform.position);
                        water_entities.push_back(e);

                        volume(x,y,z) = EntityType::WATER_BLOCK;
                    }
                }
            }
        }
    }
} // namespace cyx::island
