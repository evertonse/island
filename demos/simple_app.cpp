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
void draw_mesh(const Mesh& m) {

}
void on_mouse(Window&, f32 x, f32 y);
void on_scroll(f32 dir);

const unsigned int WIDTH = 800; 
const unsigned int HEIGHT = 600;

AccelCamera cam = AccelCamera(vec3(0.0f, 0.0f, 3.0f));
f64 delta_time = 0.0f;
auto& key_is_down  = * (new std::map<Key,bool>());

//Shader shader("assets/shaders/light.glsl");
Shader shader("assets/shaders/default.glsl");


struct MyVertex {
    f32 position[3];
    f32 normal[3];
    f32 uv[2];
};

struct SimpleMesh {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    u32 texture = 0;
    u32 vao = 0, vbo = 0,ibo = 0;
    void init() {
        // Generate and bind the vertex array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Generate and bind the vertex buffer
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Set the vertex data
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // Set the vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

        // Generate and bind the index buffer
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // Set the index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    void draw() {
        // Model Matrix
        mat4 model = mat4::identity();
        // Transformações
        model = mat4::translate(model, vec3(1,0,0));
        float angle = 25.0f;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        shader.bind();
        shader.uniform_mat4("model", model.data(), true);
        assert(shader.is_compiled());
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        std::cout << "got_here texture:" << texture  <<"vao:" << vao << "vbo:" << vbo << std::cin.get();        
    }
};

struct SimpleTexture {
    u32 id;

    u32 load(const char* filepath){
        int width, height, channels;
        unsigned char* image_data = stbi_load(filepath, &width, &height, &channels, 0);

        // Generate and bind texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &id);

        glBindTexture(GL_TEXTURE_2D, id);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image_data);
        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

std::vector<SimpleMesh> meshes;
Mesh temp;
SimpleMesh horse;
SimpleTexture tex;

auto on_create(Window& win) {
    gladLoadGL();
std::cout << "here sizeof Vertex" << sizeof(Vertex); std::cin.get();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    //tex.init("assets/textures/tiger_white.png");
    //temp = Mesh::horse();
    //temp =  Mesh::tiger();

    //std::cout << "HERE"; std::cin.get();
    
    temp = TriangularMesh::from_obj("assets/models/horse.tri.obj")->to_mesh();
    horse.vertices = temp.vertices; 
    horse.indices = temp.indices; 
    horse.texture = tex.id;
    
    //meshes.push_back(horse);
    //SimpleMesh globin;
    //globin.vertices = temp.vertices; 
    //globin.indices = temp.indices; 
    //globin.texture = tex_globin.id; 
    //meshes.push_back(globin);

   
    //meshes.push_back(Mesh::wolf());
    shader.bind();
    shader.compile();
    horse.init();
    //shader.uniform_int1("tex0", 0);
  

}

void on_update(Window& win, f64 dt) {

    for (auto& item : key_is_down) {
        if (item.second){
            cam.on_key(item.first, delta_time);
        }
    } 

    cam.on_tick(dt);
    delta_time = dt;

    glClearColor(0.8f, 0.8f, 0.9f, 0.9f);

    // Definir qual ou quais buffers precisam ser limpos
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//ativa textura
	glEnable(GL_TEXTURE_2D);
		

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);    
	
    //draw_blocks(win);
    //draw_meshes(win);

    // Projection Matrix
    shader.bind();
    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);

    shader.uniform_mat4("projection", projection.data(),true);
    
    // View Matrix
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);
    shader.uniform_mat4("view", view.data(), true);

    horse.draw();
    // Trazer os "back buffers" para frente
    win.swap_buffer();
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

void draw_meshes(Window& win) {

    shader.bind();
    // Projection Matrix
    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f);

    shader.uniform_mat4("projection", projection.data(),true);
    
    // View Matrix
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up);
    shader.uniform_mat4("view", view.data(), true);
    //shader.uniform_float3("camera_pos", cam.position.data());
    for(unsigned int i = 0; i < meshes.size(); i++) {
        // Model Matrix
        mat4 model = mat4::identity();
        // Transformações
        model = mat4::translate(model, vec3(1,0,0));
        float angle = 25.0f * i;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
        
        shader.uniform_mat4("model", model.data(),true);
        // Enviar Model Matrix para o Vertex Shader
        //shader.uniform_mat4("model", model.data(),true);
        meshes[i].draw();
    }
    glBegin(GL_TRIANGLES);
        glutWireTeapot(200);
    glEnd();
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