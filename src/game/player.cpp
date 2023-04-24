#include "game/player.hpp"

namespace island {

    void Player::tick(f32 dt) {
        auto& position = state.position;
        auto& velocity = state.velocity;

        velocity += acceleration;
        acceleration = {0, 0, 0};

        if (!flying) {
            velocity.y -= 20 * dt;
        }

        position.x += velocity.x * dt;

        position.y += velocity.y * dt;

        position.z += velocity.z * dt;

        velocity.x *= 0.90f;
        velocity.z *= 0.90f;
    }


    void Player::on_keydown(Key key) {
        float PLAYER_SPEED = 0.4f;
        if (key == Key::SHIFT) {
            PLAYER_SPEED *= 10.0f;
        }

        vec3& rotation = state.rotation;

        if (key == Key::W) {
            acceleration.x += -std::cos(radians(rotation.y + 90)) * PLAYER_SPEED;
            acceleration.z += -std::sin(radians(rotation.y + 90)) * PLAYER_SPEED;
        }
        else if (key == Key::S) {
            acceleration.x += std::cos(radians(rotation.y + 90)) * PLAYER_SPEED;
            acceleration.z += std::sin(radians(rotation.y + 90)) * PLAYER_SPEED;
        }
        if (key == Key::A) {
            acceleration += vec3::left(rotation) * PLAYER_SPEED;
        }
        else if (key == Key::D) {
            acceleration += vec3::right(rotation) * PLAYER_SPEED;
        }
        if (key == Key::SPACE && !flying) {
            acceleration.y += PLAYER_SPEED * 20;
        }
        else if (key == Key::SHIFT && flying) {
            acceleration.y -= PLAYER_SPEED * 2;
        }
        if (rotation.x < -80.0f) {
            rotation.x = -79.9f;
        }
        else if (rotation.x > 85.0f) {
            rotation.x = 84.9f;
        }
    }

    void Player::on_mouse(Window& win, int x, int y,bool lock_mouse) {
        static auto old_mouse =  veci2(x,y);
        vec3& rotation = state.rotation;
        f32 angle = 2*M_PI/180.0;

        f32 vertical_sensitivity = 1.0f;
        f32 horizontal_sensitivity = 1.0f;
        veci2 dxy = veci2(x,y) - old_mouse;
        rotation.x += static_cast<f32>(dxy.y / 8.0f * vertical_sensitivity);
        rotation.y += static_cast<f32>(dxy.x / 8.0f * horizontal_sensitivity);
        old_mouse = {x,y};
        if(lock_mouse)
            win.set_mouse((int)(win.width() / 2.0f), (int)(win.height() / 2.0f));
    }
}