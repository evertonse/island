
#include <string>
#include <map>
#include "cyx.hpp"
#include "cyx.cpp"
#include "game/camera.hpp"

using namespace cyx;

class MyApp : public Application {
    TriangularMesh* mesh;
    vec3 translation;
    vec3 perspective;
    f32 scale;
    vec3 axis;
    f32 angle;

    vec3 light_direction;
    std::map<u32,bool> is_down;
    veci2 mouse;

    bool rotate = false;
    vec3 mesh_color; 
    island::Camera cam; 

    void on_create (Window& win) override {
        // Translation, Perspective and Scale of Homogeneous Matrix
        translation = vec3(0.0, 0.0, 0.0);
        perspective = vec3(0.0, 0.05, 1.2);
        scale = 1.5;
        cam.yaw;
        // Axis and angle of quaternion rotation
        angle = 1.2;
        axis = vec3(-1, -0.5, -1);

        // Light rotation and direction to vabagundo shading
        light_direction = vec3(0.0, 2.0, 1.0);
        //light_rotation  = rotatex(0.15)

        // Color of the object being rendered
        mesh_color = vec3(255,123,123)/255.0;

        // Variable that controle if we should rotate 
        rotate = true;
        // It's faster using matrix, because it leverages the numpy library
        // But I left this here as a demonstrations that we can just multiply 
        // the quaternions and vector as usual, no problem i.e -> q v q*
        // if this variable is set to True it'll use the matrix, otherwise
        // it'll do the quaternion multiplication instead
        gladLoadGL();

        glViewport(0,0, win.height(), win.width());
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        auto clear_color = vec4(0.9,0.9,0.8,1.0);
        glClearColor(clear_color.x,clear_color.y, clear_color.z,clear_color.w); 

        
        //mesh = TriangularMesh::from_obj("assets/cat.obj", true);
        mesh = TriangularMesh::from_obj("assets/teapot.obj", false);
        //mesh = TriangularMesh::from_obj("assets/suzanne.obj", true);
        std::cout << " mesh->faces_count:"<< mesh->faces_count  << '\n';

    }

    void on_update ( Window& win, f64 dt) override {
        win.set_title( 
           "My Super App" 
        );

        f64 aspect = ((f32)win.width()) / win.height();
        f64 fov   = 75.0f;
        f64 near_ = -20.0;
        f64 far_ = 20.0;
        //gluPerspective(fov, aspect,-1.5, 10.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.5, 1.5, -1.5, 1.5, near_, far_);
        glMatrixMode(GL_MODELVIEW);
        f32 cam_matrix[16];
        cam.on_update(cam_matrix);
        glLoadMatrixf(cam_matrix);
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < mesh->faces_count; i++) {
            auto&  vertices = mesh->vertices;
            auto&  faces = mesh->faces;
            auto&  normals = mesh->normals;
            auto& v1  = vertices[faces[i].v[0]];
            auto& v2  = vertices[faces[i].v[1]];
            auto& v3  = vertices[faces[i].v[2]];

            vec3& n1  = normals[faces[i].n[0]];
            vec3& n2  = normals[faces[i].n[1]];
            vec3& n3  = normals[faces[i].n[2]];
            //std::cout << " v1:" << v1 << ' ';
            vec4  hv1 = vec4(v1 * scale, 1.0);
            vec4  hv2 = vec4(v2 * scale, 1.0);
            vec4  hv3 = vec4(v3 * scale, 1.0);
            //hv1.w = v1.z;
            //hv2.w = v2.z;
            //hv3.w = v3.z;
            //std::cout << " hv1:" << hv1 << ' ';

            light_direction.normalize();
            // Get the normal associated with this triangle face
            // Simple vabagundo shading, calculating intesity based on 
            // "sameness" of direction between light ray and normal
            f32 intensity;
            intensity = n1.dot(light_direction);
            // Calculate the color with the new acquired intensity
            vec3 color;
            color = mesh_color * std::abs(intensity);
            glColor3fv(color.data());
            glVertex4fv(hv1.data());

            intensity = vec3::dot(n2,light_direction);
            color = mesh_color * std::abs(intensity);
            glColor3fv(color.data());
            glVertex4fv(hv2.data());
            //std::cout << "v2:"<< v2 << ' ';

            intensity = vec3::dot(n3,light_direction);
            color = mesh_color * std::abs(intensity);
            glColor3fv(color.data());
            glVertex4fv(hv3.data());
            //std::cout << "v3:"<< v3 << ' ';

            //std::cout << "\n";
        }
        glEnd();
        
        //glBegin(GL_TRIANGLES);
        //glColor3f(1.0f, 0.0f, 0.0f);
        //glVertex3f(-0.5f, -0.5f, 0.0f);
        //glColor3f(0.0f, 1.0f, 0.0f);
        //glVertex3f(0.5f, -0.5f, 0.0f);
        //glColor3f(0.0f, 0.0f, 1.0f);
        //glVertex3f(0.0f, 0.5f, 0.0f);
        //glEnd();

        win.swap_buffer();
    }

    void on_event (Window& win, Event e){ 
        persistent_data bool fullscreen = false;

        if (e.type  == EventType::KEYDOWN) {
            if (e.key == Key::ESCAPE) {
                fullscreen  = !fullscreen ; 
            }
            win.set_fullscreen(fullscreen);
        }
        if (e.type == EventType::RESHAPE) {
            glViewport(0,0, win.width(), win.height());
        }
        if (e.type == EventType::MOUSEWHEEL) {
            if (e.mouse.wheel.dir > 0)
                scale += 0.3;
            else 
                scale -= 0.1;
        }
        cam.on_key(e.key);
        cam.on_mouse(e.mouse.x,e.mouse.y);

    }
};

auto main(int argc, char* argv[] ) ->  int {
    auto app = MyApp();
    Window* win   = new GlutWindow(&app);
    win->start();

    return 0;
}