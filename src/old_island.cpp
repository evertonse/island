
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
    bool mouse_is_down = false;
} flag;


#include <iostream>
using std::cout;
using std::endl;

void init_gl();
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
VertexBuffer* VBO;
VertexArray* VAO;

auto& key_is_down = * (new std::map<Key,bool>());

Shader shader(
    "assets/shaders/default.glsl"
);

Shader light(
    "assets/shaders/light.glsl"
);


SimpleTexture tex_wall;
SimpleTexture tex_horse;
SimpleTexture tex_goblin;
SimpleTexture tex_tiger;

TripleBufferMesh goblin;
TripleBufferMesh tiger;
TripleBufferMesh cube;

//TriangularMesh tri_mesh;

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
    //goblin.load("assets/models/goblin/goblin.obj"); 
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
    tex_goblin.load("assets/textures/tex_goblin.png");
    tex_tiger.load("assets/textures/tiger_white.png");

    TripleBufferMesh::goblin(&goblin);
    TripleBufferMesh::tiger(&tiger);

    // Desvincular VBO e VAO para não modificar acidentalmente
    VAO->unbind();
    VBO->unbind();
    // Habilitar teste do buffer de profundidade

    shader.bind();
    shader.compile();
    light.bind();
    light.compile();

}

void on_destroy(Window& win) {

    //glDeleteVertexArrays(1, &VAO); // Opcional
    //glDeleteBuffers(1, &VBO); // Opcional
}
    // Loop de renderização principal
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

    // Vincular textura
    tex_wall.bind();

    // Definir qual Shader Program o OpenGL deve usar
    shader.bind();
    // Projection Matrix
    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);

    shader.uniform_mat4("projection", projection.data(),true);
    shader.uniform_mat4("view", view.data(), true);

    light.bind();

    light.uniform_mat4("projection", projection.data(),true);
    light.uniform_mat4("view", view.data(), true);
    
    //std::cout << " Fragment: " << light.src(ShaderType::FRAGMENT) << "Vertex : " << light.src(ShaderType::VERTEX); std::cin.get();


    // Trazer os "back buffers" para frente
    mat4 model = mat4::identity();
    model = mat4::translate(model, vec3(-3.8f, -2.0f, -2.3f));
    float angle = -15.0f ;
    model = mat4::rotate(model, radians(angle), vec3(5.0f, 0.3f, 0.5f));
    tex_horse.bind();
    light.bind();
    light.uniform_mat4("model", model.data(),true);
    shader.bind();
    shader.uniform_mat4("model", model.data(),true);
    light.bind();

    tex_goblin.bind();
    goblin.draw();

    //tri_mesh.draw();


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


    std::cout << "horse : " << tex_horse.id << "\n";
    std::cout << "wall : " << tex_wall.id << "\n";
    

    VAO->bind();
    shader.bind();
    for(unsigned int i = 0; i < 10; i++) {
        // Model Matrix
        if (i  == 2 || i == 3){
            //std::cout << "le fuck  i = " << i << '\n'; std::cin.get();
            tex_goblin.bind();
        }else {
            tex_horse.bind();
        }


        mat4 model = mat4::identity();
        model = mat4::translate(model, cubePositions[i]);
        float angle = -20.0f * i;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        
        // Enviar Model Matrix para o Vertex Shader
        shader.uniform_mat4("model", model.data(),true);

        // Desenhar triângulos a partir dos vertices
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, vi.count(), GL_UNSIGNED_INT, NULL);
    }
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

        //if(flag.mouse_lock)
            //win.set_mouse((int)(w / 2.0f), (int)(h / 2.0f));
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

int main() {
    Application app;
    
    app.on_init = [](Window&){std::cout << "glut init\n";};
    app.on_create = on_create;
    app.on_update = on_update;
    app.on_event  = on_event;
    app.on_destroy = on_destroy;
    Window* win   = new GlutWindow(&app);
    win->set_width(WIDTH);
    win->set_height(HEIGHT);
    win->start();
}
