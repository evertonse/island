
#include <string>
#include <map>
#include "cyx.hpp"
#include "cyx.cpp"
#include "utils/log.hpp"

using namespace cyx;
using namespace island;

class MyApp : public Application {
    bool lock_mouse = true;
    struct Plane {
        f64 
            left = -1.0, right = 1.0,
            bottom = -1.0, top = 1.0,
            znear = -10.0, zfar = 10.0;
        friend std::ostream& operator<<(std::ostream& os, Plane& rhs) {
            os  << "left:" << rhs.left << " right:" << rhs.right << "\n"  
                << "bottom:" << rhs.bottom << " top:" << rhs.top << "\n"  
                << "near:" << rhs.znear << " far:" << rhs.zfar << "\n"; 
            return os;
        }
    };

    island::Player player;

    island::Camera cam; 

    std::vector<TriangularMesh*> meshes;
    vec3 translation{0};
    vec3 perspective{0};
    f32 scale;
    vec3 axis{0};
    f32 angle;
    Plane plane;
    vec3 light_direction;
    std::map<u32,bool> is_down;
    veci2 mouse;

    bool rotate = false;
    vec3 mesh_color; 

    void on_create (Window& win) override {
        // Translation, Perspective and Scale of Homogeneous Matrix
        translation = vec3(0.0, 0.0, 0.0);
        perspective = vec3(0.0, 0.05, 1.2);
        scale = 1.5;

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
        player.state.position = {0,0,0};
        player.state.rotation = {0,0,0};
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

        
        meshes.push_back(TriangularMesh::from_obj("assets/teapot.obj", true));
        meshes.push_back(TriangularMesh::from_obj("assets/suzanne.obj", true));

    }

    void on_update ( Window& win, f64 dt) override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        win.set_title( 
           "My Super App" 
        );

        player.tick(dt);

        f64 aspect = ((f32)win.width()) / win.height();
        f64 fov   = 50.0f;
        out << plane;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //glLoadMatrixf(mat4::perspective(fov,aspect,20,20).transpose().data());
        //glOrtho(plane.left, plane.right,plane.bottom,plane.top,plane.znear,plane.zfar);
        //gluPerspective(fov,aspect,plane.znear,plane.zfar);
        //glFrustum(plane.l, plane.r, plane.b, plane.t, plane.n, plane.f);
        out << "\naspect:" <<aspect << " fov: " << fov << " \n";

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // camera transforms:
        glTranslatef(-cam.position.x, -cam.position.y, -cam.position.z);
        //glRotatef(-player.state.rotation.x, 0, 1, 0); 
        //gluLookAt(
            //cam.position.x, cam.position.y, cam.position.z,
            //0.0, 0.0, 0.0,
            //0.0, 1.0, 0.0
        //);
        //glLoadMatrixf(cam.to_matrix().transpose().data());

        out << "\nplayer rotation " << player.state.rotation << " ;";
        out << "player position " << player.state.position << " ;\n";
        //cam.direction = vec3(player.state.rotation);
        cam.position  = vec3(player.state.position);
        out << "cam direction " << cam.direction << " ;";
        out << "cam position  " << cam.position << " ;\n";
        mat4 matrix_cam = cam.to_matrix().transpose();
        //glLoadMatrixf(matrix_cam.data());

        glBegin(GL_TRIANGLES);
        out << plane;
        //glutSolidSphere(9, 200, 200);
        auto t_vec = vec3(0,0,2);
        for (auto& mesh : meshes) {
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


                vec4  hv1 = vec4(v1 * scale + t_vec, 1.0f) ;
                vec4  hv2 = vec4(v2 * scale + t_vec, 1.0f) ;
                vec4  hv3 = vec4(v3 * scale + t_vec, 1.0f) ;
                //out << "\n hv1:" << hv1 << " \n"
                    //<< " hv2:" << hv2 << " \n"
                    //<< " hv3:" << hv3 << " \n";


                light_direction.normalize();
                // Get the normal associated with this triangle face
                // Simple vabagundo shading, calculating intesity based on 
                // "sameness" of direction between light ray and normal
                
                f32 intensity;
                vec3 pontual_light;
                // Calculate the color with the new acquired intensity
                vec3 color;
                pontual_light = (light_direction - v1);
                intensity = n1.dot(pontual_light);
                //intensity = n1.dot(light_direction);
                color = mesh_color * std::abs(intensity);
                glColor3fv(color.data());
                glVertex4fv(hv1.data());

                //intensity = vec3::dot(n2,light_direction);
                pontual_light = (light_direction - v2);
                intensity = n2.dot(pontual_light);
                color = mesh_color * std::abs(intensity);
                glColor3fv(color.data());
                glVertex4fv(hv2.data());
                //std::cout << "v2:"<< v2 << ' ';

                //intensity = vec3::dot(n3,light_direction);
                pontual_light = (light_direction - v3);
                intensity = n3.dot(pontual_light);
                color = mesh_color * std::abs(intensity);
                glColor3fv(color.data());
                glVertex4fv(hv3.data());
                //std::cout << "v3:"<< v3 << ' ';

                //std::cout << "\n";
            }
        }
        glEnd();
        
        out << "done";
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.5f, 0.0f);
        glEnd();

        win.swap_buffer();
    }

    void on_event (Window& win, Event e){ 
        persistent_data bool fullscreen = false;
        persistent_data veci2 old_size(win.width(), win.height());
        if (e.type  == EventType::KEYDOWN) {
            player.on_keydown(e.key);
            if (e.key == Key::ESCAPE) {
                fullscreen  = !fullscreen ; 
                if (fullscreen) {
                    old_size.width  = win.width();
                    old_size.height = win.height();
                    //win.set_fullscreen(true);
                    this->lock_mouse = !lock_mouse;
                }
                else {
                    win.set_height(old_size.height);
                    win.set_width(old_size.width);
                }
            }
            if (e.key == Key::SPACE) {
            }
        }
        if (e.type == EventType::RESHAPE) {
            glViewport(0,0, win.width(), win.height());
            old_size = {(i32)win.width(), (i32)win.height()};
        }
        if (e.type == EventType::MOUSEWHEEL) {
            if (e.mouse.wheel.dir > 0)
                scale += 0.3;
            else 
                scale -= 0.1;
        }
        //cam.on_key(e.key);
        //cam.on_mouse(e.mouse.x,e.mouse.y)o;
        
        player.on_mouse(win,e.mouse.x, e.mouse.y, lock_mouse);

    }
};

auto main(int argc, char* argv[] ) ->  int {
    auto app = MyApp();
    Window* win   = new GlutWindow(&app);
    win->start();

    return 0;
}