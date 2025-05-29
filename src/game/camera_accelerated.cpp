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

    const f32 pitch_clamp = 89.0f;
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
    AccelCamera(AccelCamera&& other) noexcept = delete;
    // Copy assignment operator
    AccelCamera& operator=(const AccelCamera& other) = delete;
    // Move assignment operator
    AccelCamera& operator=(AccelCamera&& other) noexcept = delete;
    

    mat4 to_view_matrix() const {
        return mat4::lookat(this->position, this->position + this->direction, this->up);
    }

    void update_vectors() {
        // Calculate the direction based on yaw and pitch 
        vec3 dir;
        dir.x = -std::cos(radians(this->yaw)) * std::cos(radians(this->pitch));
        dir.y = -std::sin(radians(this->pitch));
        dir.z = -std::sin(radians(this->yaw)) * std::cos(radians(this->pitch));
        //  create a orthonormal basis for the camera
        this->direction = dir.normalize();
        this->right = vec3::cross(this->direction, this->world_up).normalize();  
        this->up = vec3::cross(this->right, this->direction).normalize();
    }

    void on_tick(f32 dt) {
        // Simple formula for updating position
        // velocity is increasing by the  current acceleration
        // and acceleration goes to zero
        // then we finally update the position based on delta time
        auto& position = this->position;
        auto& velocity = this->velocity;

        velocity += acceleration * dt;
        acceleration = {0, 0, 0};

        position += velocity;

        // Simulate damping of velocity by "atrito" from the ground
        velocity *= 0.956f;
    }

    void on_key(Key key, f32 dt) {
        float SPEED = 5.05f * speed * dt;

        persistent_data Key key_before = key;
        if (key == Key::SHIFT) {
            SPEED *= 15.0f * speed * dt;
        }

        // We use determine the left vetor for the correct yaw
        // and we add 90 to the yaw to get the forward vector and we add to the
        // acceleration, the same for the backwards vector
        if (key == Key::W) {
            acceleration += vec3::left(vec3(pitch, yaw + 90.0f, 0.0f)) * SPEED;
        }
        else if (key == Key::S) {
            acceleration += vec3::right(vec3(pitch, yaw + 90.0f, 0.0f)) * SPEED;
        }
        // Here, we dont need to add the 90f because left is just left
        else if (key == Key::A) {
            acceleration += vec3::left(vec3(pitch, yaw, 0.0f)) * SPEED;
        }
        else if (key == Key::D) {
            acceleration += vec3::right(vec3(pitch, yaw, 0.0f)) * SPEED;
        }
        // NOTE: CRTL, SHIFT is bugged right now, we have no crtl keyup event
        else if (key == Key::E ) {
            acceleration.y -= SPEED * 2;
        }
        // else if (key == Key::Q || key == Key::SPACE){
        else if (key == Key::SPACE || key == Key::Q) {
            acceleration.y += SPEED * 2.198;
        }
        key_before = key;
    }

    void on_mouse(f64 xoffset, f64 yoffset, bool locked = false) {
        // This funciton take in an offset from the
        // last x and y compared to the current one
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;
        // we don't use this lock logic anymore
        if (locked) {
            this->yaw  = xoffset;
            this->pitch = yoffset;
        }
        // If the mouse went up, our pitch goes up
        // if the mouse wen sideways the yaw acts accordinly
        else {
            this->yaw += xoffset;
            this->pitch += yoffset;
        }
        // Clamp to avoid flipping of the camera.
        // simply wont let the pitch go too up
        // nor too much down
        this->pitch = clamp(pitch, -pitch_clamp ,pitch_clamp );
        update_vectors();
    }
};
}
