#pragma once
#include <cmath>
#include <numbers>
#include "math/vec.hpp"
#include "graphics/event.hpp"

using namespace cyx;
namespace island {
    
//  Default Values
const f32 YAW = -90.0f;
const f32 PITCH = 0.0f;

const f32 SPEED = 5.3f;
const f32 SENSITIVITY = 0.25f;
const f32 FOV = 75.0f;

struct Camera {

    vec3 position;
    vec3 direction;
    vec3 right;
    vec3 up;

    vec3 velocity{0,0};
    vec3 acceleration{0,0};
    vec3 world_up;

    f32 yaw;
    f32 pitch;
    
    f32 speed;
    f32 fov;

    f32 mouse_sensitivity;

    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : direction(vec3(0.0f, 0.0f, -1.0f)), speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
    {   this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        update_vectors();
    }

    Camera() = default; 
    Camera(const Camera& other) = default;
    // Move constructor
    Camera(Camera&& other) noexcept = default;
    // Copy assignment operator
    Camera& operator=(const Camera& other) = default;
    // Move assignment operator
    Camera& operator=(Camera&& other) noexcept = default;
    
    mat4 to_matrix() const {
        _todo("Aint sure it works");
        // Page 165
        vec3 dir   = dir.normalize();
        vec3 up    = vec3::cross({0,1,0},dir).normalize();
        vec3 right = vec3::cross(dir, up).normalize();

        
        auto& w = dir;
        auto& v = right;
        auto& u = up;

        mat4 rotation = mat4({
            { u.x,  u.y,  u.z,  0.0f },
            { v.x,  v.y,  v.z,  0.0f },
            { w.x,  w.y,  w.z,  0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        });
        auto& e = this->position;
        mat4 translation = mat4::translation(-e);
        return rotation % translation;
    } 

    mat4 to_view_matrix() const {
        return mat4::lookat(this->position, this->position + this->direction, this->up);
    }

    void update_vectors() {
        vec3 front;
        front.x = std::cos(radians(this->yaw)) * std::cos(radians(this->pitch));
        front.y = std::sin(radians(this->pitch));
        front.z = std::sin(radians(this->yaw)) * std::cos(radians(this->pitch));
        this->direction = front.normalize();
        this->right = vec3::cross(this->direction, this->world_up).normalize();  
        this->up = vec3::cross(this->right, this->direction).normalize();
    }

    void on_key(Key key, f32 dt) {
        f32 velocity = speed * dt;
        if (key == Key::W)
            this->position += this->direction * velocity;
        if (key == Key::S)
            this->position -= this->direction * velocity;
        if (key == Key::A)
            this->position -= this->right * velocity;
        if (key == Key::D)
            this->position += this->right * velocity;
        if (key == Key::SPACE)
            this->position.y += velocity;
        if (key == Key::SHIFT)
            this->position.y -= velocity;
    }

    void on_mouse(f64 x, f64 y) {

        f64 xoffset = x * mouse_sensitivity;
        f64 yoffset = y * mouse_sensitivity;
        this->yaw += xoffset;
        this->pitch += yoffset;
        
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
        update_vectors();
    }
};
}