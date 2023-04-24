#pragma once
#include <cmath>
#include <numbers>
#include "math/vec.hpp"
#include "graphics/event.hpp"

using namespace cyx;
namespace island {
    
struct Camera {

    vec3 position;
    vec3 direction;
    vec3 rotation; // we're using rotation as a 3 value degree for x-axis y-axis and z-axis respectively
    
    f32 pitch, yaw, speed = 0.1f;
    veci2 old_mouse;
    mat4 projection = mat4::identity();
    mat4 projection_view = mat4::identity();

    Camera() = default; 
    Camera(const Camera& other) = default;
    // Move constructor
    Camera(Camera&& other) noexcept = default;
    // Copy assignment operator
    Camera& operator=(const Camera& other) = default;
    // Move assignment operator
    Camera& operator=(Camera&& other) noexcept = default;

    Camera(u32 width, u32 height) {
        projection =
            mat4::perspective(3.14f / 2.0f, width / height, 0.01f, 2000.0f);
    }

    void on_mouse(int x, int y) {
        // calculate the difference between the current mouse position and the last position
        int dx = x - old_mouse.x;
        int dy = y - old_mouse.y;
        
        // update the camera yaw and pitch based on the mouse movement
        yaw += dx * 0.1f;
        pitch += dy * 0.1f;
        
        // clamp the pitch to prevent the camera from flipping upside down
        f32 pitch_clamp = 90.0f/2;
        //f32 pitch_clamp = M_PI;
        if (pitch > pitch_clamp) pitch = pitch_clamp;
        if (pitch < -pitch_clamp) pitch = -pitch_clamp;
            
        // update the old_mouse. position
        old_mouse.x = x;
        old_mouse.y = y;
    }

    void on_update(f32 matrix[16]) {

        auto& dir = this->direction;
        // change x to z and z to x if something goes wrong
        f32 factor = M_PI / 180.0f;
        f32 cos_yaw   = std::cos(yaw * factor);
        f32 cos_pitch = std::cos(pitch * factor);
        f32 sin_yaw   = std::sin(yaw * factor);
        f32 sin_pitch = std::sin(pitch * factor);
        dir.x = cos_pitch * cos_yaw;
        dir.y = sin_pitch;
        dir.z = sin_yaw * cos_pitch;
        // normalize the direction vector
        //dir = dir.normalize();
        
        // calculate the camera's right vector by cross product of direction vector and the global up vector
        vec3 right = vec3::cross(dir, vec3(0.0f,1.0f,0.0f)).normalize();

        // calculate the camera's up vector by taking the cross product of the right vector and the direction vector
        vec3 up = vec3::cross(dir,right).normalize();
        
        // update the OpenGL modelview matrix with the new camera position and orientation
        auto& m  = matrix; 
        auto& cam = this->position; 
        m[0] = right.x; m[4] = up.x; m[8]  = -dir.x; m[12] = 0.0f,
        m[1] = right.y; m[5] = up.y; m[9]  = -dir.y; m[13] = 0.0f,
        m[2] = right.z; m[6] = up.z; m[10] = -dir.z; m[14] = 0.0f,
        m[3] = 0.0f;    m[7] = 0.0f; m[11] =  0.0f;  m[15] = 1.0f;
    }
    
    void on_update(const EntityState& state) {
        this->position       = state.position;
        this->rotation       = state.rotation;
        auto& degrees = this->rotation;

        mat4 view  = mat4::identity();
        mat4 projection_view_  = mat4::identity();

        view = mat4::rotate(view, radians(degrees.x), {1, 0, 0});
        view = mat4::rotate(view, radians(degrees.y), {0, 1, 0});
        view = mat4::rotate(view, radians(degrees.z), {0, 0, 1});
        view = mat4::translate(view, vec3((this->position)*(-1)) );
        this->projection_view = projection % view;
    }

    void on_key(Key key) {

        auto& dir = this->direction;
        auto& pos = this->position;
        switch (key) {
            case Key::W:
                pos.x += speed * dir.x;
                pos.y += speed * dir.y;
                pos.z += speed * dir.z;
            break;
            case Key::S:
                pos.x -= speed * dir.x;
                pos.y -= speed * dir.y;
                pos.z -= speed * dir.z;
            break;
            case Key::A :
                pos.x += speed * sin(yaw - M_PI/2);
                pos.y += speed * cos(yaw - M_PI/2);
            break;
            case Key::D :
                pos.x -= speed * sin(yaw - M_PI/2);
                pos.y -= speed * cos(yaw - M_PI/2);
            break;
            
            default:
            break;
        }
    }
};
}