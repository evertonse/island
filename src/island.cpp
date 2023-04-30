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

const unsigned int WIDTH = 800; 
const unsigned int HEIGHT = 600;

AccelCamera cam = AccelCamera(vec3(10.0f, 40.0f, 10.0f), vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);

VertexBuffer* VBO;
VertexArray* VAO;

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

Shader light(
    "assets/shaders/light.glsl"
);

Shader water(
    "assets/shaders/water.glsl"
);

std::vector<Entity> entities;
SimpleTexture tex_wall;
SimpleTexture tex_horse;
SimpleTexture tex_goblin;
SimpleTexture tex_tiger;

TripleBufferMesh tiger;
TripleBufferMesh cube;

// The skybox is better commented in its header file
// and basically creates a sky effect, a light bluish sky
Skybox skybox;
// The world containing all its entities
Model goblin;

// Might be a bit heavy to keepit on the stack so we leave on the heap
World& world = *(new World());

// src input file for the world
const char* world_file = "src/input.txt";
// Vertices dos triângulos
float vertices[] =
{
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// Definir a posição dos cubos no espaço do mundo
vec3 cubePositions[] = {
    vec3( 0.0f,  0.0f,  0.0f),
    vec3( 2.0f,  5.0f, -15.0f),
    vec3(-1.5f, -2.2f, -2.5f),
    vec3(-3.8f, -2.0f, -12.3f),
    vec3( 2.4f, -0.4f, -3.5f),
    vec3(-1.7f,  3.0f, -7.5f),
    vec3( 1.3f, -2.0f, -2.5f),
    vec3( 1.5f,  2.0f, -2.5f), 
    vec3( 1.5f,  0.2f, -1.5f), 
    vec3(-1.3f,  1.0f, -1.5f)
};

auto on_create(Window& win) {
    init_gl();
    // Vertex Buffer Object

    // Vertex Array Object

    // Vincular VAO, VBO e EBO
    VAO = new VertexArray();
    VBO = new VertexBuffer();
    VAO->bind();

    VBO->bind();
    // Copiar dados dos vertices para o VBO vinculado
    VBO->set_data(vertices, sizeof(vertices));

    VertexLayout layout;
    layout.push( VertexAttr::Float3 );
    layout.push( VertexAttr::Float2 );

    VAO->add(*VBO,layout);

    tex_wall.load("assets/textures/wall.jpg");
    tex_horse.load("assets/textures/tex_horse.png");
    tex_tiger.load("assets/textures/tiger_white.png");
    goblin.texture.load("assets/textures/tex_goblin.png");
    TripleBufferMesh::goblin(&goblin.mesh);
    TripleBufferMesh::tiger(&tiger);
    TripleBufferMesh::cube(&cube);
    entities.push_back(Entity::make(EntityType::TERRESTRIAL1));
    entities.push_back(Entity::make(EntityType::TERRESTRIAL1));
    entities.push_back(Entity::make(EntityType::TERRAIN_BLOCK));
    entities.push_back(Entity::make(EntityType::TERRAIN_BLOCK));
    entities.push_back(Entity::make(EntityType::TERRESTRIAL2));
    entities.push_back(Entity::make(EntityType::PLANT1));
    entities.push_back(Entity::make(EntityType::PLANT2));
    entities.push_back(Entity::make(EntityType::TERRESTRIAL2));
    // Desvincular VBO e VAO para não modificar acidentalmente
    VAO->unbind();
    VBO->unbind();
    // Habilitar teste do buffer de profundidade

    shader.bind();
    shader.compile();
    light.bind();
    light.compile();
    water.bind();
    water.compile();

    // Read the World from file input
    world.from_file(world_file);
    // Generate the volume that defines the world and its water
    world.generate_volume();
    world.generate_water();
    // Init the sky box with the chosen sky box
    // In the definition the this #define there is another option
    // for a cloudy skybox instead of the bluish sky
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
    tex_wall.bind();

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
    
    // Pass delta_time to make the water move
    persistent_data f32 time = dt;
    time += dt;
    water.bind();
    water.uniform_mat4("projection",  projection.data(),true);
    water.uniform_mat4("view",        view.data(), true);
    water.uniform_vec3("cam_position",cam.position.data());
    water.uniform_float("time", time);


    light.bind();
    for(auto& e : world.entities) {
        mat4 model = mat4::identity();
        if(world.is_movable(e)) {
           model = mat4::rotate(model, radians(e.transform.angle), {0.0, 1.0, 0.0});
        } 
        model = mat4::scale(model, e.transform.scale);
        // The width of a block is 2 so we need to translate by 2
        // but since it has some z-value fightin we spread them apart
        // by a small factor of 000.1
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        light.uniform_mat4("model", model.data(),true);
        e.model->draw();
    }

    light.bind();
    for(auto& e : world.entities) {
        mat4 model = mat4::identity();
        // If movable we need to rotate to the corrected orientation
        if(world.is_movable(e)) {
           model = mat4::rotate(model, radians(e.transform.angle), {0.0, 1.0, 0.0});
        } 
        model = mat4::scale(model, e.transform.scale);
        // The width of a block is 2 so we need to translate by 2
        // but since it has some z-value fightin we spread them apart
        // by a small factor of 000.1
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        light.uniform_mat4("model", model.data(),true);
        e.model->draw();
    }

    // Draw the water
    water.bind();
    for(auto& e : world.water_entities) {
        mat4 model = mat4::identity();
        model = mat4::scale(model, e.transform.scale);
        model = mat4::translate(model, vec3(e.transform.position)*2.001);
        water.uniform_mat4("model", model.data(),true);
        e.model->draw();
    }


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



    tex_tiger.bind();
    tiger.draw();

    VAO->bind();
    VBO->bind();
    shader.bind();
    for(unsigned int i = 0; i < 10; i++) {
        // Model Matrix
        mat4 model = mat4::identity();
        // Transformações
        model = mat4::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        
        // Enviar Model Matrix para o Vertex Shader
        shader.uniform_mat4("model", model.data(),true);

        // Desenhar triângulos a partir dos vertices
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, vi.count(), GL_UNSIGNED_INT, NULL);
    }

    

    VAO->bind();
    shader.bind();
    for(unsigned int i = 0; i < 10; i++) {
        if (i  == 2 || i == 3){
            tex_goblin.bind();
        }
        else {
            tex_horse.bind();
        }
        mat4 model = mat4::identity();
        model = mat4::translate(model, cubePositions[i]);
        float angle = -20.0f * i;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        shader.uniform_mat4("model", model.data(),true);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
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
    Window* win   = new GlutWindow(&app);
    win->set_width(WIDTH);
    win->set_height(HEIGHT);
    win->start();
}
