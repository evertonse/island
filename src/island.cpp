#include <string>
#include <map>
#include <iostream>

#define CYX_POINTER_BASED_APPLICATION
#include "cyx.hpp"
#include "cyx.cpp"
#include "utils/log.hpp"

using namespace cyx;
using namespace island;

struct Flags {
    bool mouse_lock = false; 
    bool mouse_is_down = false;
} flag;



void init_gl();
void update_title(Window& win, f32 dt);
void on_scroll(f32 dir);

void on_mouse(Window&, f32 x, f32 y);
void on_scroll(f32 dir);

const unsigned int WIDTH = 750; 
const unsigned int HEIGHT = 600;

AccelCamera cam = AccelCamera(vec3(10.0f, 40.0f, 10.0f), vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);

// To make it fluid we take into account if the key is down ourself
// instead of relying on 'key repeat' signal from the OS
auto& key_is_down = * (new std::map<Key,bool>());

// Shader for each type that we need, shader is just a basic one
// 'light' is the shader that takes into acount the camera position and calculate
// both diffuse and specular light
// 'water' takes into account the delta time to compute a sine and cosine
// to the sampling of a texture, givven the effect of a wave, also
// make the mesh transparent
Shader shader(
    "assets/shaders/default.glsl"
);

#if defined(ISLAND_ENABLE_SHADOW) && ISLAND_ENABLE_SHADOW  == true
Shader light(
    "assets/shaders/light_and_shadow.glsl"
);
#else
Shader light(
    "assets/shaders/light.glsl"
);
#endif

Shader water(
    "assets/shaders/water.glsl"
);

Shader ocean(
    "assets/shaders/ocean.glsl"
);


std::vector<Entity> entities;
SimpleTexture tex_horse;
TripleBufferMesh horse;

SimpleTexture tex_ocean;
TripleBufferMesh mesh_ocean;

// The skybox is better commented in its header file
// and basically creates a sky effect, a light bluish sky
Skybox skybox;
#if defined(ISLAND_ENABLE_SHADOW) && ISLAND_ENABLE_SHADOW  == true
ShadowMap shadow;
#endif
// The world containing all its entities

// Might be a bit heavy to keepit on the stack so we leave on the heap
World& world = *(new World());

// src input file for the world
const char* world_file = "src/input.txt";
// Vertices dos triângulos

// Definir a posição dos cubos no espaço do mundo
;

auto on_create(Window& win) {
    init_gl();

    tex_horse.load("assets/textures/tex_horse.png");
    TripleBufferMesh::horse(&horse);


    tex_ocean.load(ISLAND_OCEAN_TEXTURE);
    TripleBufferMesh::surface(&mesh_ocean);

    shader.bind();
    shader.compile();
    light.bind();
    light.compile();
    water.bind();
    water.compile();
    ocean.bind();
    ocean.compile();

    // Read the World from file input
    world.from_file(world_file);
    // Generate the volume that defines the world and its water
    world.generate_volume();
    world.generate_water();
    // Init the sky box with the chosen sky box
    // In the definition the this #define there is another option
    // for a cloudy skybox instead of the bluish sky
#if defined(ISLAND_ENABLE_SHADOW) && ISLAND_ENABLE_SHADOW  == true
    shadow.init();
#endif
    skybox.init(ISLAND_SKYBOX,".png");
}



void on_update(Window& win, f64 dt) {
    // Update title base on the delta time
    update_title(win,dt);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // For each key that is down we call the camera on_key function
    for (auto& item : key_is_down) {
        if (item.second){
            cam.on_key(item.first, dt);
        }
    }
    // We tick to update the position of the camera based on its acceleration
    cam.on_tick(dt);
    // Tick the position of the entities of the world
    world.tick_positions(dt);
    // Vincular textura

    // Projection Matrix from camera FOV it takes into 
    // account the width and height plus near and far planes
    mat4 projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);
    // Calculate the camera transformation matrix, more comments on the definition of lookat
    mat4 view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);

    // Bind vairous shaders which and send the data as uniform to it
    shader.bind();
    shader.uniform_mat4("projection", projection.data(),true);
    shader.uniform_mat4("view", view.data(), true);

    light.bind();
    light.uniform_mat4("projection",  projection.data(),true);
    light.uniform_mat4("view",        view.data(), true);
    light.uniform_vec3("cam_position",cam.position.data());

#if defined(ISLAND_ENABLE_SHADOW) && ISLAND_ENABLE_SHADOW  == true
    light.uniform_mat4("shadow_mvp",  shadow.matrix().data(), true);
    shadow.bind_texture();
    light.uniform_int("tex_shadow_map", 1);
#endif    
    // Pass delta_time to make the water move
    persistent_data f32 time = dt;
    time += dt;
    water.bind();
    water.uniform_mat4("projection",  projection.data(),true);
    water.uniform_mat4("view",        view.data(), true);
    water.uniform_vec3("cam_position",cam.position.data());
    water.uniform_float("time", time);

    ocean.bind();
    ocean.uniform_mat4("projection",  projection.data(),true);
    ocean.uniform_mat4("view",        view.data(), true);
    ocean.uniform_vec3("cam_position",cam.position.data());
    ocean.uniform_float("time", time);


#if defined(ISLAND_ENABLE_SHADOW) && ISLAND_ENABLE_SHADOW  == true
    shadow.begin();
    for(auto& e : world.movable_entities) {
        mat4 model = mat4::identity();
        // It is movable so we need to rotate to the corrected orientation
        model = mat4::rotate(model, radians(e.transform.angle), {0.0, 1.0, 0.0});
        model = mat4::scale(model, e.transform.scale);
        // The width of a block is 2 so we need to translate by 2
        // but since it has some z-value fightin we spread them apart
        // by a small factor of 000.1
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        //shadow.shader.uniform_mat4("model", model.data(),true);
        e.model->draw();
    }
    shadow.end(veci2(win.width(), win.height()));
#endif

    light.bind();
    for(auto& e : world.entities) {
        mat4 model = mat4::identity();
        model = mat4::scale(model, e.transform.scale);
        // The width of a block is 2 so we need to translate by 2
        // but since it has some z-value fightin we spread them apart
        // by a small factor of 000.1
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        light.uniform_mat4("model", model.data(),true);
        e.model->draw();
    }


    light.bind();
    for(auto& e : world.movable_entities) {
        mat4 model = mat4::identity();
        // It is movable so we need to rotate to the corrected orientation
        model = mat4::rotate(model, radians(e.transform.angle), {0.0, 1.0, 0.0});
        model = mat4::scale(model, e.transform.scale);
        // The width of a block is 2 so we need to translate by 2
        // but since it has some z-value fightin we spread them apart
        // by a small factor of 000.1
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        light.uniform_mat4("model", model.data(),true);
        e.model->draw();
        //tex_horse.bind();
        //horse.draw();
    }

    // Draw the water
    water.bind();
    if (world.water_entities.size() > 0) {
        // only bind the water once, it is faster like this
        world.water_entities[0].model->texture.bind();
    }
    for(auto& e : world.water_entities) {
        mat4 model = mat4::identity();
        model = mat4::scale(model, e.transform.scale);
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        water.uniform_mat4("model", model.data(),true);
        e.model->mesh.draw();

    }

    mat4 model = mat4::identity();
    model = mat4::scale(model, 200.0);
    model = mat4::translate(model,  vec3(0.0f, (f32)world.water_level, 0.0f)*2.0001f);
    ocean.bind();
    ocean.uniform_mat4("model", model.data(),true);
    tex_ocean.bind();
    mesh_ocean.draw();



    light.bind();
    int i = 0;
    for (auto& e : entities) {
        mat4 model = mat4::identity();
        model = mat4::translate(model, vec3(-2.8f + i, -2.0f, -2.3f));
        float angle = -15.0f + i;
        model = mat4::rotate(model, radians(angle), vec3(5.0f, 0.3f, 0.5f));
        light.uniform_mat4("model", model.data(),true);
        e.model->draw();
        i++;
    }



    // For last we draw the skybox
    skybox.draw(view,projection);
    win.swap_buffer();
}
    
// Check for key ups and down + mouse
// Update the key_is_down and flags
void on_event(Window& win, Event e){

    if (e.type == EventType::KEYDOWN){
        key_is_down[e.key] = true;
        if(e.key == Key::ESCAPE) {
           flag.mouse_lock = false; 
        }
    }
    if (e.type == EventType::KEYUP) {
        key_is_down[e.key] = false;
    }
    if (e.type == EventType::MOUSEDOWN) {
        flag.mouse_lock = true;
        flag.mouse_is_down = true;
    }    
    else if (e.type == EventType::MOUSEUP) {
        flag.mouse_is_down = false;
    }   
    if (e.type == EventType::MOUSEMOVE) {

        on_mouse(win,(f32)e.mouse.x, f32(e.mouse.y));
    }
    if (e.type == EventType::RESHAPE) {
        glViewport(0, 0 ,win.width(), win.height());
    }
    if (e.type == EventType::MOUSEWHEEL) {
        on_scroll(e.mouse.wheel.dir);
    }
}

// Optional
void on_destroy(Window& win) {
    // Put whatever it need to happen when the window is destroyed
}


// Calculate offset  before passing to the camera on_mouse
void on_mouse(Window& win, f32 xpos, f32 ypos) {
    persistent_data f32 xlast = 0.0f;
    persistent_data f32 ylast = 0.0f;
    persistent_data bool first_time = true;
    persistent_data bool handled = false;

    
    xpos = static_cast<f32>(xpos);
    ypos = static_cast<f32>(ypos);
    if (handled) {
        return;
    }
    if (first_time) {
        xlast = xpos;
        ylast = ypos;
        first_time = false;
    }

    float xoffset = xpos - xlast;
    float yoffset = ylast - ypos;
    xlast = xpos;
    ylast = ypos;
    // Make sure the mouse is currently clicking
    if (flag.mouse_is_down)
        cam.on_mouse(xoffset,yoffset);

}

// Update de FOV when we scroll
void on_scroll(f32 dir) {
    auto& fov = cam.fov;
    fov -= (f32)dir;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 105.0f)
        fov = 105.0f;
}

// Init OpenGL configurations and call the gladLoader
void init_gl(){
    gladLoadGL();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glEnable(GL_MULTISAMPLE);

	glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

// Update title with fps
void update_title(Window& win, f32 dt) {
	std::string fps = std::to_string((1.0 / dt));
	std::string title = "Island  " + fps + " fps";
    win.set_title(title.c_str());
}

// Main expects a file input, if not it will use a default input.txt
int main(int argc, const char* argv[]) {
    Application app;

    if (argc > 1) {
        world_file = argv[1];
        std::cout << "[main]: Started from input file:  " << world_file << " that description will be used to create the scene \n" ;
    }
    else {
        std::cout << "[main]: Started with no input file. The default input file:" << world_file <<" will be used" <<'\n';
    } 
    app.on_init = [](Window&){std::cout << "[Window]: glut init\n";};
    app.on_create = on_create;
    app.on_update = on_update;
    app.on_event  = on_event;
    app.on_destroy = on_destroy;
    Window* win   = new GlutWindow(&app, "Island", WIDTH, HEIGHT );
    win->set_width(WIDTH);
    win->set_height(HEIGHT);
    win->start();
}
