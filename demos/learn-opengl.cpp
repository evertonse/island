
    // set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};
unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int VBO, VAO, EBO;
#define CYX_POINTER_BASED_APPLICATION
#include "cyx.hpp"
#include "cyx.cpp"
#include "utils/log.hpp"
#include <string>
#include <map>

#include "assets/all_obj.h"

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

Shader shader;

const char * vertex_shader =
    R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)";

const char * fragment_shader =
R"(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D u_tex;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = texture(u_tex, TexCoord);
	FragColor = vec4(.2,.7,.2,.7);
}
)";


struct SimpleTexture {
    u32 id;

    u32 init(const char* filepath){
    // load and create a texture 
    // -------------------------
    
    // texture 1
    // ---------
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id); 
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

};
SimpleTexture texture;
auto on_create(Window& win) {
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    shader = Shader::from_src(vertex_shader, fragment_shader);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //texture.init("assets/textures/wall.jpg");
    texture.init("assets/textures/tex_globin.png");
    shader.bind(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    //shader.uniform_int1("beautiful", (int)0);
}

void on_update(Window& win, f64 dt) {

    for (auto& item : key_is_down) {
        if (item.second){
            cam.on_key(item.first, delta_time);
        }
    } 


    glClearColor(0.8f, 0.8f, 0.9f, 0.9f);

    // Definir qual ou quais buffers precisam ser limpos
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        // render container
        shader.bind();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   

    win.swap_buffer();
}

    

void on_destroy(Window& win) {

}

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

