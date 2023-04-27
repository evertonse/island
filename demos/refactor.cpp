
#define CYX_POINTER_BASED_APPLICATION
#include "cyx.hpp"
#include "cyx.cpp"
#include "utils/log.hpp"
#include <string>
#include <map>

using namespace cyx;
using namespace island;

struct Flags {
    bool mouse_lock = false; // doesnt work for now
    bool mouse_is_down = false; // doesnt work for now
} flag;


#include <iostream>
using std::cout;
using std::endl;

void on_scroll(f32 dir);

void on_mouse(Window&, f32 x, f32 y);
void on_scroll(f32 dir);

const unsigned int WIDTH = 800; 
const unsigned int HEIGHT = 600;

vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

AccelCamera cam = AccelCamera(vec3(0.0f, 0.0f, 3.0f));
f64 delta_time = 0.0f;

Texture* tex_block;
Texture* tex_dragon;
Texture* tex_fox;

VertexBuffer* VBO;
VertexArray* VAO;

auto& key_is_down  = * (new std::map<Key,bool>());

Shader shader(
    "assets/shaders/vertex_shader.glsl", 
    "assets/shaders/fragment_shader.glsl"
);


Shader light_shader("assets/shaders/light.glsl");

std::vector<Mesh> meshes;
// Vertices dos triângulos
// Vertices dos triângulos
float vertices[] = {
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

    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    meshes.push_back(Mesh::tiger());
    meshes.push_back(Mesh::wolf());
    light_shader.bind();
    light_shader.compile();
}

auto init(Window& win) {

    gladLoadGL();

    //meshes.push_back(TriangularMesh::from_obj("assets/teapot.obj", true)->to_mesh());
    //meshes.push_back(TriangularMesh::from_obj("assets/suzanne.obj", true)->to_mesh());
    //meshes.push_back(Mesh::horse());
    //meshes.push_back(Mesh::bamboo());
    //meshes.push_back(Mesh::wolf());

    //meshes.push_back(TriangularMesh::from_obj("assets/models/cube/cube.obj", true)->to_mesh());
    //meshes.push_back(TriangularMesh::from_obj("assets/CartoonDragon.obj", true)->to_mesh());
    //meshes.push_back(TriangularMesh::from_obj("assets/models/fox.obj", true)->to_mesh());
    //meshes.push_back(TriangularMesh::from_obj("assets/models/wolf.tri.obj", true)->to_mesh());
    //meshes.push_back(TriangularMesh::from_obj("assets/models/cube/cube.obj", true)->to_mesh());
    // Vertex Buffer Object

    // Vertex Array Object

    // Vincular VAO, VBO e EBO
    ///// Definir parâmetros de quebra de textura
    ///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    ///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ///// Definir parâmetros de filtragem de textura
    ///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenerateMipmap(GL_TEXTURE_2D);
    // Desvincular VBO e VAO para não modificar acidentalmente
    VAO = new VertexArray();
    VBO = new VertexBuffer();

    tex_block  = new Texture("assets/textures/wall.jpg");

    VAO->bind();
    VBO->bind();
    // Copiar dados dos vertices para o VBO vinculado
    VBO->set_data(vertices, sizeof(vertices));

    VertexLayout layout;
    layout.push( VertexAttr::Float3 );
    layout.push( VertexAttr::Float2 );

    VAO->add(*VBO,layout);


    shader.bind();
    shader.compile();
    VBO->unbind();
    VAO->unbind();
    // Habilitar teste do buffer de profundidade
}

void on_destroy(Window& win) {

    //glDeleteVertexArrays(1, &VAO); // Opcional
    //glDeleteBuffers(1, &VBO); // Opcional
}
    // Loop de renderização principal
void on_event(Window& win, Event e){

    if (e.type == EventType::KEYDOWN){
        key_is_down[e.key] = true;
        //win.set_cursor(Cursor::NONE);
        flag.mouse_lock = false; 
        if(e.key == Key::ESCAPE) {
            //win.set_cursor(Cursor::ARROW);
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

void draw_blocks(Window& win) {

    // Vincular textura
    tex_block->bind();

    // Definir qual Shader Program o OpenGL deve usar
    shader.bind();

    // Projection Matrix
    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);
    // might need to transpose
    shader.uniform_mat4("projection", projection.data(),true);

    // View Matrix
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);
    shader.uniform_mat4("view", view.data(), true);

    // Vincular Vertex Array Object
    VAO->bind();
    VBO->bind();

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
    tex_block->unbind();
    VBO->unbind();
    VAO->unbind();
}

void draw_meshes(Window& win) {

    light_shader.bind();
    // Projection Matrix
    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);

    light_shader.uniform_mat4("projection", projection.data(),true);
    
    // View Matrix
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);
    light_shader.uniform_mat4("view", view.data(), true);

    for(unsigned int i = 0; i < meshes.size(); i++) {
        // Model Matrix
        mat4 model = mat4::identity();
        // Transformações
        model = mat4::translate(model, cubePositions[i]);
        float angle = 25.0f * i;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        
        light_shader.uniform_mat4("model", model.data(),true);
        // Enviar Model Matrix para o Vertex Shader
        //light_shader.uniform_mat4("model", model.data(),true);
        meshes[i].draw(light_shader);
    }
}

void on_update(Window& win, f64 dt) {

    for (auto& item : key_is_down) {
        if (item.second){
            cam.on_key(item.first, delta_time);
        }
    }
    cam.on_tick(dt);
    delta_time = dt;
    // Especificar os valores de vermelho, verde, azul e alfa, para limpar os buffers de cores
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Definir qual ou quais buffers precisam ser limpos
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw_blocks(win);
    draw_meshes(win);
    

    // Trazer os "back buffers" para frente
    win.swap_buffer();
}

    

void on_mouse(Window& win, f32 xposIn, f32 yposIn) {
    persistent_data f32 lastX = 0.0f;
    persistent_data f32 lastY = 0.0f;
    persistent_data bool first_time = true;
    persistent_data bool handled = false;

    f32 xpos = static_cast<f32>(xposIn);
    f32 ypos = static_cast<f32>(yposIn);
    if (handled) {
        return;
    }
    if (first_time) {
        lastX = xpos;
        lastY = ypos;
        first_time = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    if (!first_time && flag.mouse_lock) {
        u32 w = win.width();
        u32 h = win.height();

        //yoffset = f32((yoffset - h/2)/h);
        //xoffset = f32((xoffset - w/2)/w);
        //xoffset = f32((ypos - h/2)/h);
        //yoffset = f32((xpos - w/2)/w);

        //xoffset = f32((ypos - h/2)/h);
        //yoffset = f32((xpos - w/2)/w);
        //first_time = true;
        if(flag.mouse_lock){
            //win.set_mouse((int)(w / 2.0f), (int)(h / 2.0f));
        }
    }

    if (flag.mouse_is_down)
        cam.on_mouse(xoffset,yoffset);

}

void on_scroll(f32 dir) {
    auto& fov = cam.fov;
    fov -= (f32)dir;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 105.0f)
        fov = 105.0f;
}

int main() {
    Application app;
    app.on_create = on_create;
    app.on_update = on_update;
    app.on_event  = on_event;
    app.on_destroy = on_destroy;
    Window* win   = new GlutWindow(&app);
    win->set_width(WIDTH);
    win->set_height(HEIGHT);
    win->start();
}