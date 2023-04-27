#pragma once
#include <cmath>
#include "math/vec.hpp"
#include "math/mat.hpp"
#include "graphics/event.hpp"
#include "game/defaults.h"

using namespace cyx;
namespace island {
    

struct AccelCamera {

    vec3 position;
    vec3 direction;
    vec3 right;
    vec3 up;

    vec3 velocity{0,0};
    vec3 acceleration{0,0};
    vec3 world_up{0,1.0f,0.0};

    f32 yaw;
    f32 pitch;
    
    f32 speed;
    f32 fov;

    f32 mouse_sensitivity;

    AccelCamera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : direction(vec3(0.0f, 0.0f, -1.0f)), speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
    {   this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        update_vectors();
    }

    AccelCamera() = default; 
    AccelCamera(const AccelCamera& other) = default;
    // Move constructor
    AccelCamera(AccelCamera&& other) noexcept = default;
    // Copy assignment operator
    AccelCamera& operator=(const AccelCamera& other) = default;
    // Move assignment operator
    AccelCamera& operator=(AccelCamera&& other) noexcept = default;
    

    mat4 to_view_matrix() const {
        return mat4::lookat(this->position, this->position + this->direction, this->up);
    }

    void update_vectors() {
        vec3 dir;
        dir.x = std::cos(radians(this->yaw)) * std::cos(radians(this->pitch));
        dir.y = std::sin(radians(this->pitch));
        dir.z = std::sin(radians(this->yaw)) * std::cos(radians(this->pitch));
        this->direction = dir.normalize();
        this->right = vec3::cross(this->direction, this->world_up).normalize();  
        this->up = vec3::cross(this->right, this->direction).normalize();
    }

    void on_tick(f32 dt) {
        auto& position = this->position;
        auto& velocity = this->velocity;

        velocity += acceleration;
        acceleration = {0, 0, 0};

        position.x += velocity.x * dt;

        position.y += velocity.y * dt;

        position.z += velocity.z * dt;

        velocity.x *= 0.90f;
        velocity.z *= 0.90f;
        velocity.y *= 0.90f;
    }

    void on_key(Key key, f32 dt) {
        float PLAYER_SPEED = 5.4f * speed * dt;
        if (key == Key::SHIFT) {
            PLAYER_SPEED *= 15.0f * speed * dt;
        }
        if (key == Key::A) {
            acceleration.x += -std::cos(radians(yaw + 90)) * PLAYER_SPEED;
            acceleration.z += -std::sin(radians(yaw + 90)) * PLAYER_SPEED;
        }
        else if (key == Key::D) {
            acceleration.x += std::cos(radians(yaw + 90)) * PLAYER_SPEED;
            acceleration.z += std::sin(radians(yaw + 90)) * PLAYER_SPEED;
        }
        else if (key == Key::S) {
            acceleration += vec3::left(vec3(pitch,yaw,0.0)) * PLAYER_SPEED;
        }
        else if (key == Key::W) {
            acceleration += vec3::right(vec3(pitch,yaw,0.0)) * PLAYER_SPEED;
        }
        else if ((key == Key::E) || (key == Key::CTRL)) {
            acceleration.y -= PLAYER_SPEED * 2;
        }
        else if (key == Key::Q || key == Key::SPACE){
            acceleration.y += PLAYER_SPEED * 2.1;
        }    

        if (pitch < -80.0f) {
            pitch = -79.9f;
        }
        else if (pitch > 85.0f) {
            pitch = 84.9f;
        }
    }

    void on_mouse(f64 x, f64 y, bool locked = false) {
        f64 xoffset = x * mouse_sensitivity;
        f64 yoffset = y * mouse_sensitivity;
        if (locked) {
            this->yaw  = xoffset;
            this->pitch = yoffset;
        }
        else {
            this->yaw += xoffset;
            this->pitch += yoffset;
        }
        
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
        update_vectors();
    }
};
}