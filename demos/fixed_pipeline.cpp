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


struct SimpleMesh {
    static void draw(
        f32* vertices,
        f32* normals, f32* uvs, 
        u32* indices, u32 indices_count,
        u32 texture
    ) {
        // Model Matrix
        mat4 model = mat4::identity();
        // Transformações
        model = mat4::translate(model, vec3(1,0,0));
        float angle = 25.0f;
        model = mat4::rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f)).transpose();
            
        //primitivas usam cores diretamente da textura
        //Exercício: atribuir cores aos vértices e testar outros modos 
        //onde eles também são levados em conta
        //ativa textura
        glEnable(GL_TEXTURE_2D);
        // Bind the texture to the texture unit
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindTexture(GL_TEXTURE_2D, texture);
	
        glPushMatrix();
        
        glLoadMatrixf(model.data());
        

        //Mecanismo similar ao glPushMatrix
        //no topo da pilha é colocada uma cópia de um conjunto de atributos,
        //incluindo a cor
        glPushAttrib(GL_CURRENT_BIT);
            
        //ativa arrays que serão usados
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        //associa dados aos arrays
        glVertexPointer  (3, GL_FLOAT, 0, vertices);
        glNormalPointer  (GL_FLOAT,  0,  normals);
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);
        //Formas de desenho

        //1.Desenho por vértice
        /*
        glBegin(GL_TRIANGLE_FAN);
                glArrayElement (0);
                glArrayElement (1);
                glArrayElement (2);
                glArrayElement (3);
        glEnd();
        */
            
        //2. Desenho na ordem definida por um array de índices -> glBegin e glEnd implícitos  
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, indices);
        glDrawArrays(GL_TRIANGLES, 0, indices_count);
        
        
        //3. Desenho na ordem de declaração -> glBegin e glEnd implícitos
        //glDrawArrays(GL_TRIANGLE_FAN, 1, 3);
        
        
        //retira topo da pilha de atributos
        glPopAttrib();
        //retira a matriz de transformação do topo da pilha
        glPopMatrix();

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
    }


struct SimpleTexture {
    u32 id;

    u32 init(const char* filepath){
        // Inverter as texturas carregadas no eixo y
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* image_data = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);

    //declara um objeto de textura
        glGenTextures(1, &id);
        
        //cria e usa objetos de textura 
        //podem ser trocados ou removidos (remoção: texName=0)
        glBindTexture(GL_TEXTURE_2D, id);
        
        //todas as alterações feitas a seguir afetam o objeto associado
        //à textura
        
        //como a textura será tratada se não há mapeamento direto 
        //entre pixels e coordenadas de textura (GL_REPEAT ou GL_CLAMP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        //magnification e minification filters
        //GL_NEAREST: texel com coordenadas mais próximas do centro do pixel 
        //é usado
        //testar GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        //define uma textura bidimensional
        /*void glTexImage2D(
        * GLenum target, 
        * GLint level, 
        * GLint internalFormat, (número de componentes)
        * GLsizei width, 
        * GLsizei height, 
        * GLint border,
        * GLenum format, 
        * GLenum type,    (tipos dos dados)
        * const GLvoid *pixels);*/


        // Load texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);


        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

std::vector<SimpleMesh> meshes;
SimpleTexture tex_globin1;

auto on_create(Window& win) {
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    tex_globin1.init("assets/textures/wall.jpg");
    const int texWidth = 2;
    const int texHeight = 2;
    const unsigned char textureData[] =
    {
        255, 255, 255, 255,
        0,   0,   0,   255,
        0,   0,   0,   255,
        255, 255, 255, 255
    };

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set up the view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Enable depth testing and lighting
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Set the light position and color
    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    // Define the vertex data for a cube
    const std::vector<float> positions =
    {
        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back face
        -0.5f, -0.5f, -0.5f,
        -0.5f,  
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

   
   

    mat4 projection = mat4::identity();  
    projection = mat4::perspective(radians(cam.fov), (float)win.width()/ (float)win.height(), 0.1f, 100.0f).transpose();

    
    // View Matrix
    mat4 view = mat4::identity();  
    view = mat4::lookat(cam.position, cam.position + cam.direction, cam.world_up).transpose();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(projection.data());

    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(view.data());

    SimpleMesh::draw(
        (f32*)goblin_objVerts,
        (f32*)goblin_objNormals,
        (f32*)goblin_objTexCoords,
        (u32*)goblin_objIndexes,
        (u32)goblin_objIndexesCount,
        tex_globin1.id
    ); 

    SimpleMesh::draw(
        (f32*)horse_objVerts,
        (f32*)horse_objNormals,
        (u32*)horse_objIndexes,
        (u32)horse_objIndexesCount,
        tex_globin1.id
    );
    // Trazer os "back buffers" para frente
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
