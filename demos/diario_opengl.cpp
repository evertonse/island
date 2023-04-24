
#define CYX_POINTER_BASED_APPLICATION
#include "cyx.hpp"
#include "cyx.cpp"
#include "utils/log.hpp"

#include <string>
#include <map>

using namespace cyx;
using namespace island;


struct Flags {
    bool mouse_lock = true;
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

AccelCamera cam(cameraPosition,cameraUp);
f64 delta_time = 0.0f;
unsigned int texture;
unsigned int VBO;
unsigned int VAO;

auto& key_is_down = * (new std::map<Key,bool>());

Shader shader(
    "assets/shaders/vertex_shader.glsl", 
    "assets/shaders/fragment_shader.glsl"
);

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
vec3 cubePositions[] =
{
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
    // Vertex Buffer Object
    glGenBuffers(1, &VBO);

    // Vertex Array Object
    glGenVertexArrays(1, &VAO);

    // Vincular VAO, VBO e EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copiar dados dos vertices para o VBO vinculado
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atribuir ponteiros para os vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atribuir ponteiros para as texuturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Textura
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Definir parâmetros de quebra de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Definir parâmetros de filtragem de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Inverter as texturas carregadas no eixo y
    stbi_set_flip_vertically_on_load(true);
    
    // Carregar textura
    int width, height, channels;
    unsigned char *data = stbi_load("assets/textures/wall.jpg", &width, &height, &channels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Falha ao carregar textura" << endl;
    }
    stbi_image_free(data);

    // Desvincular VBO e VAO para não modificar acidentalmente
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Habilitar teste do buffer de profundidade
    glEnable(GL_DEPTH_TEST);

    shader.compile();
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
    }    
   if (e.type == EventType::MOUSEMOVE) {
        on_mouse(win,(f32)e.mouse.x, f32(e.mouse.y));
        //win.set_mouse((int)(win.width() / 2.0f), (int)(win.height() / 2.0f));
    }
    if (e.type == EventType::RESHAPE) {
        glViewport(0, 0 ,win.width(), win.height());
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
    glBindTexture(GL_TEXTURE_2D, texture);

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
    glBindVertexArray(VAO);

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
    }

    // Trazer os "back buffers" para frente
    win.swap_buffer();
}
    


void on_mouse(Window& win, f32 xposIn, f32 yposIn)
{
    persistent_data bool first_time = true;
    persistent_data f32 lastX = 0.0f;
    persistent_data f32 lastY = 0.0f;
    f32 xpos = static_cast<f32>(xposIn);
    f32 ypos = static_cast<f32>(yposIn);

    if (first_time ) {
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
        win.set_mouse((int)(w / 2.0f), (int)(h / 2.0f));
        cam.on_mouse(xoffset,yoffset);
        return;
    }
    cam.on_mouse(xoffset,yoffset);

}


void on_scroll(f32 dir) {
    auto& fov = cam.fov;
    fov -= (float)dir;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
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